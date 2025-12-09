#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using Id = unsigned long long;
using IdRange = std::pair<Id, Id>;

class InputData
{
public:
    std::deque<IdRange> freshIds;
    std::vector<Id> availableIds;
};

InputData readInputFile(std::string const &filePath)
{
    bool blankLineEncountered = false;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return {};
    }

    InputData inputData;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            blankLineEncountered = true;
            continue;
        }

        if (!blankLineEncountered)
        {
            size_t dashPos = line.find('-');
            if (dashPos == std::string::npos)
                continue;

            Id first = std::stoull(line.substr(0, dashPos));
            Id second = std::stoull(line.substr(dashPos + 1));
            inputData.freshIds.push_back({first, second});
        }
        else
        {
            Id id = std::stoull(line);
            inputData.availableIds.push_back(id);
        }
    }

    return inputData;
}

size_t countFreshIds(const InputData &inputData)
{
    size_t count = 0;
    for (const auto &id : inputData.availableIds)
    {
        bool isFresh = false;
        for (const auto &range : inputData.freshIds)
        {
            if (id >= range.first && id <= range.second)
            {
                isFresh = true;
                std::cout << "ID " << id << " is fresh (in range " << range.first << "-" << range.second << ")" << std::endl;
                break;
            }
        }
        if (isFresh)
        {
            count++;
        }
    }
    return count;
}

bool removeOverlappingRanges(std::deque<IdRange> &ranges)
{
    bool removedAny = false;
    for (auto it = ranges.begin(); it != ranges.end(); ++it)
    {
        for (auto jt = it + 1; jt != ranges.end();)
        {
            if ((jt->first >= it->first && jt->first <= it->second) ||
                (jt->second >= it->first && jt->second <= it->second) ||
                (it->first >= jt->first && it->first <= jt->second) ||
                (it->second >= jt->first && it->second <= jt->second))
            {
                it->first = std::min(it->first, jt->first);
                it->second = std::max(it->second, jt->second);
                jt = ranges.erase(jt);
                removedAny = true;
            }
            else
            {
                ++jt;
            }
        }
    }

    return removedAny;
}

size_t stripAndCountTotalFreshIds(std::deque<IdRange> &freshIds)
{
    while (removeOverlappingRanges(freshIds))
    {
        // keep removing overlapping ranges until none are left
    }

    size_t total = 0;
    for (const auto &range : freshIds)
    {
        total += (range.second - range.first + 1);
        std::cout << "Fresh ID range: " << range.first << "-" << range.second << " : Count " << (range.second - range.first + 1) << std::endl;
    }
    return total;
}

int main()
{
    auto inputData = readInputFile("input/input.txt");
    size_t freshCount = stripAndCountTotalFreshIds(inputData.freshIds);
    std::cout << "Number of fresh IDs: " << freshCount << std::endl;
    return 0;
}
