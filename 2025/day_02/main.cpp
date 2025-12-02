#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using IdType = long long;
std::vector<std::pair<IdType, IdType>> readInputFile(std::string const &filePath)
{
    std::vector<std::pair<IdType, IdType>> data;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return data;
    }

    std::string line;
    std::getline(file, line);

    size_t pos = 0;
    while (pos < line.length())
    {
        size_t dashPos = line.find('-', pos);
        if (dashPos == std::string::npos)
            break;

        size_t commaPos = line.find(',', dashPos);
        if (commaPos == std::string::npos)
            commaPos = line.length();

        IdType first = std::stoll(line.substr(pos, dashPos - pos));
        IdType second = std::stoll(line.substr(dashPos + 1, commaPos - dashPos - 1));
        data.push_back({first, second});
        std::cout << "Read " << first << "-" << second << std::endl;
        pos = commaPos + 1;
    }

    file.close();
    return data;
}

bool isInvalidPart1(IdType id)
{
    // find the invalid IDs by looking for any ID which is made only of some
    // sequence of digits repeated twice. So, 55 (5 twice), 6464 (64 twice), and
    // 123123 (123 twice) would all be invalid IDs.

    std::string idStr = std::to_string(id);
    size_t len = idStr.length();

    if (len % 2 != 0)
        return false; // must be even length

    std::string firstHalf = idStr.substr(0, len / 2);
    std::string secondHalf = idStr.substr(len / 2, len / 2);
    return firstHalf == secondHalf;
}

bool isInvalidPart2(IdType id)
{
    // an ID is invalid if it is made only of some sequence of digits repeated
    // at least twice. So, 12341234 (1234 two times), 123123123 (123 three
    // times), 1212121212 (12 five times), and 1111111 (1 seven times) are all
    // invalid IDs.

    std::string idStr = std::to_string(id);
    size_t len = idStr.length();

    for (size_t patternLength = 1; patternLength <= len / 2; patternLength++)
    {
        if (len % patternLength != 0)
            continue; // must divide evenly

        std::string pattern = idStr.substr(0, patternLength);
        size_t numRepeats = len / patternLength;
        std::string constructed;
        for (size_t i = 0; i < numRepeats; i++)
        {
            constructed += pattern;
        }
        if (constructed == idStr)
            return true;
    }
    return false;
}

int main()
{
    IdType invalidIdSum = 0;
    std::vector<std::pair<IdType, IdType>> data = readInputFile("input/input.txt");
    for (auto const &p : data)
    {
        for (IdType i = p.first; i <= p.second; ++i)
        {
            if (isInvalidPart2(i))
            {
                std::cout << "Invalid ID: " << i << std::endl;
                invalidIdSum += i;
            }
        }
    }

    std::cout << "Sum of invalid IDs: " << invalidIdSum << std::endl;
    return 0;
}
