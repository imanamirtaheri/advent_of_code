#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class ProblemData
{
public:
    char operand;
    std::vector<int> numbers;
};

std::vector<ProblemData> readInputFilePart1(std::string const &filePath)
{
    std::vector<ProblemData> data;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        bool operandLine = (line[0] == '+' || line[0] == '*');

        size_t index = 0U;
        size_t pos = 0U;
        while (pos < line.length())
        {
            // Skip leading whitespace
            while (pos < line.length() && std::isspace(line[pos]))
            {
                pos++;
            }

            if (pos >= line.length())
                break;

            size_t spacePos = line.find(' ', pos);
            if (spacePos == std::string::npos)
                spacePos = line.length();

            if (index >= data.size())
            {
                data.push_back(ProblemData{});
            }

            auto subStr = line.substr(pos, spacePos - pos);

            // Skip empty substrings
            if (subStr.empty())
            {
                pos = spacePos + 1;
                continue;
            }

            std::cout << "Read substring: '" << subStr << "'" << std::endl;
            if (operandLine)
            {
                data[index].operand = subStr[0];
            }
            else
            {
                int number = std::stoi(subStr);
                data[index].numbers.push_back(number);
            }

            pos = spacePos + 1;
            index++;
        }
    }

    file.close();
    return data;
}

std::vector<ProblemData> readInputFilePart2(std::string const &filePath)
{
    std::vector<ProblemData> data;
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return data;
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        lines.push_back(line);
    }

    size_t lineCount = lines.size();
    size_t lineLength = lines[0].length();

    std::vector<int> problemNumbers;
    for (int i = lineLength - 1U; i >= 0; i--)
    {
        int number = 0;
        bool allEmpty = true;
        for (size_t j = 0U; j < lineCount; j++)
        {
            char ch = lines[j][i];
            if (j == lineCount - 1 && !allEmpty)
            {
                std::cout << "Read number: " << number << " at line " << j << std::endl;
                problemNumbers.push_back(number);
            }

            if (std::isdigit(ch))
            {
                allEmpty = false;
                number = number * 10 + (ch - '0');
            }
            else if (ch == '+' || ch == '*')
            {
                ProblemData problem;
                problem.operand = ch;
                problem.numbers = problemNumbers;
                data.push_back(problem);
                problemNumbers.clear();
            }
        }
    }

    file.close();
    return data;
}

long long computeResult(const std::vector<ProblemData> &problems)
{
    long long result = 0;
    for (const auto &problem : problems)
    {
        long long partialResult = (problem.operand == '+') ? 0 : 1;
        for (const auto &number : problem.numbers)
        {
            if (problem.operand == '+')
            {
                partialResult += number;
            }
            else if (problem.operand == '*')
            {
                partialResult *= number;
            }
        }
        std::cout << "Partial result for operand " << problem.operand << ": " << partialResult << std::endl;
        result += partialResult;
    }
    return result;
}

int main()
{
    auto inputData = readInputFilePart2("input/input.txt");
    auto result = computeResult(inputData);
    std::cout << "Total result: " << result << std::endl;
    return 0;
}
