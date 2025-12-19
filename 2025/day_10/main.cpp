#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <climits>

struct Problem
{
    uint32_t desiredState;
    std::vector<uint32_t> switchMask;
    std::vector<uint32_t> desiredJoltage;
};

std::vector<Problem> readInputFile(const std::string &filePath)
{
    std::vector<Problem> data;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        Problem p;
        size_t closingBrace = line.find(']');
        p.desiredState = 0U;
        for (size_t i = 1; i < closingBrace; ++i)
            if (line[i] == '#')
                p.desiredState |= (1U << (i - 1));
        size_t pos = closingBrace + 1;
        while (pos < line.size())
        {
            if (line[pos] == '(')
            {
                size_t close = line.find(')', pos);
                uint32_t mask = 0U;
                size_t ipos = pos + 1;
                while (ipos < close)
                {
                    size_t comma = line.find(',', ipos);
                    if (comma == std::string::npos || comma > close)
                        comma = close;
                    size_t idx = std::stoul(line.substr(ipos, comma - ipos));
                    mask |= (1U << idx);
                    ipos = comma + 1;
                }
                p.switchMask.push_back(mask);
                pos = close + 1;
            }
            else if (line[pos] == '{')
            {
                size_t close = line.find('}', pos);
                size_t ipos = pos + 1;
                while (ipos < close)
                {
                    size_t comma = line.find(',', ipos);
                    if (comma == std::string::npos || comma > close)
                        comma = close;
                    p.desiredJoltage.push_back(std::stoul(line.substr(ipos, comma - ipos)));
                    ipos = comma + 1;
                }
                pos = close + 1;
            }
            else
                pos++;
        }
        data.push_back(p);
    }
    return data;
}

size_t minTogglesBFS(uint32_t desiredState, const std::vector<uint32_t> &switchMasks)
{
    std::vector<std::pair<uint32_t, size_t>> q;
    std::vector<bool> visited(1U << 20, false);
    q.push_back({0U, 0});
    visited[0] = true;
    size_t head = 0;
    while (head < q.size())
    {
        auto [state, steps] = q[head++];
        if (state == desiredState)
            return steps;
        for (auto mask : switchMasks)
        {
            uint32_t next = state ^ mask;
            if (!visited[next])
            {
                visited[next] = true;
                q.push_back({next, steps + 1});
            }
        }
    }
    return SIZE_MAX;
}

// Iterative deepening DFS to find minimum button presses
size_t minButtonPressesJoltage(const Problem &problem)
{
    const auto &buttons = problem.switchMask;
    size_t n = problem.desiredJoltage.size();
    size_t k = buttons.size();
    const auto &target = problem.desiredJoltage;

    // Calculate lower bound: max target value
    size_t lowerBound = *std::max_element(target.begin(), target.end());
    
    // Iterative deepening: try cost limits from lowerBound upwards
    for (size_t costLimit = lowerBound; costLimit < lowerBound + 300; ++costLimit)
    {
        std::vector<int> current(n, 0);
        bool found = false;

        // DFS within cost limit
        std::function<bool(size_t, int)> dfs = [&](size_t buttonIdx, int remainingCost) -> bool {
            if (found)
                return true;

            // Check if we reached the target
            bool match = true;
            for (size_t i = 0; i < n; ++i)
            {
                if (current[i] != (int)target[i])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                found = true;
                return true;
            }

            if (buttonIdx == k || remainingCost == 0)
                return false;

            // Calculate max presses for this button without overshooting
            int maxPresses = remainingCost;
            for (size_t i = 0; i < n; ++i)
            {
                if (buttons[buttonIdx] & (1U << i))
                {
                    int room = (int)target[i] - current[i];
                    if (room < 0)
                    {
                        maxPresses = -1;
                        break;
                    }
                    maxPresses = std::min(maxPresses, room);
                }
            }

            if (maxPresses < 0)
                return false;

            // Try pressing 0 to maxPresses times
            for (int p = 0; p <= maxPresses; ++p)
            {
                // Apply
                for (size_t i = 0; i < n; ++i)
                {
                    if (buttons[buttonIdx] & (1U << i))
                        current[i] += p;
                }

                if (dfs(buttonIdx + 1, remainingCost - p))
                    return true;

                // Undo
                for (size_t i = 0; i < n; ++i)
                {
                    if (buttons[buttonIdx] & (1U << i))
                        current[i] -= p;
                }
            }
            return false;
        };

        if (dfs(0, costLimit))
            return costLimit;
    }

    return SIZE_MAX;
}

void part1(const std::vector<Problem> &inputData)
{
    size_t total = 0U;
    for (size_t i = 0; i < inputData.size(); ++i)
    {
        auto toggles = minTogglesBFS(inputData[i].desiredState, inputData[i].switchMask);
        total += toggles;
    }
    std::cout << "Part 1: Total minimum toggles: " << total << std::endl;
}

void part2(const std::vector<Problem> &inputData)
{
    size_t total = 0U;
    for (size_t i = 0; i < inputData.size(); ++i)
    {
        auto presses = minButtonPressesJoltage(inputData[i]);
        if (presses != SIZE_MAX)
            total += presses;
    }
    std::cout << "Part 2: Total minimum button presses: " << total << std::endl;
}

int main()
{
    auto inputData = readInputFile("input/input.txt");
    part1(inputData);
    part2(inputData);
    return 0;
}
