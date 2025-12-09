#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

class ProblemData
{
public:
    size_t initialPosition;
    std::vector<std::set<size_t>> splitters;
};

ProblemData readInputFile(std::string const &filePath)
{
    ProblemData data;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return data;
    }

    std::string line;

    // First line contains the initial position
    std::getline(file, line);
    data.initialPosition = line.find('S');

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::set<size_t> splitterPositions;
        for (size_t i = 0; i < line.length(); i++)
        {
            if (line[i] == '^')
            {
                splitterPositions.insert(i);
            }
        }
        data.splitters.push_back(splitterPositions);
    }

    file.close();
    return data;
}

std::pair<size_t, size_t> processInput(const ProblemData &data)
{
    size_t totalNumberOfSplits = 0U;
    std::unordered_map<size_t, size_t> activeBeams = {{data.initialPosition, 1U}};

    for (const auto &splitterRow : data.splitters)
    {
        std::unordered_map<size_t, size_t> nextActiveBeam;

        for (const auto &activeBeam : activeBeams)
        {
            auto addBeam = [&](size_t pos)
            {
                auto it = nextActiveBeam.find(pos);
                if (it == nextActiveBeam.end())
                {
                    nextActiveBeam.emplace(pos, activeBeam.second);
                }
                else
                {
                    it->second += activeBeam.second;
                }
            };

            if (splitterRow.find(activeBeam.first) != splitterRow.end())
            {
                addBeam(activeBeam.first - 1U);
                addBeam(activeBeam.first + 1U);
                totalNumberOfSplits++;
            }
            else
            {
                addBeam(activeBeam.first);
            }
        }

        activeBeams = std::move(nextActiveBeam);
    }

    size_t totalTimeLines = 0U;
    for (const auto &activeBeam : activeBeams)
    {
        totalTimeLines += activeBeam.second;
    }

    return {totalNumberOfSplits, totalTimeLines};
}
int main()
{
    auto inputData = readInputFile("input/input.txt");
    auto result = processInput(inputData);
    std::cout << "Total number of splits: " << result.first << std::endl;
    std::cout << "Total time lines: " << result.second << std::endl;
    return 0;
}
