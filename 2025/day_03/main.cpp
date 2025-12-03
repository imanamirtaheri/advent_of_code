#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

constexpr size_t BATTERIES_COUNT = 12;

uint64_t getBankJoltage(std::string const &bank)
{
    size_t bankSize = bank.size();
    std::vector<char> selectedBatteries;
    selectedBatteries.reserve(BATTERIES_COUNT);

    for (size_t bankIndex = 0; bankIndex < bankSize; bankIndex++)
    {
        // We can pop only a certain number of selected batteries. If we remove
        // too much, we won't be able to form a full size pack
        int removableBatteriesCount = selectedBatteries.size() + bankSize - bankIndex - BATTERIES_COUNT;
        while ((!selectedBatteries.empty()) && (removableBatteriesCount > 0) && (bank[bankIndex] > selectedBatteries.back()))
        {
            selectedBatteries.pop_back();
            removableBatteriesCount--;
        }

        if (selectedBatteries.size() != BATTERIES_COUNT)
        {
            selectedBatteries.push_back(bank[bankIndex]);
        }
    }

    uint64_t joltage = 0U;
    for (auto const &battery : selectedBatteries)
    {
        joltage = joltage * 10 + battery - '0';
    }
    return joltage;
}

uint64_t processInputFile(std::string const &filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 0;
    }

    uint64_t joltageSum = 0U;
    std::string bank;
    while (std::getline(file, bank))
    {
        if (bank.empty())
            continue;

        auto bankJoltage = getBankJoltage(bank);
        std::cout << "Bank: " << bank << " -> Joltage: " << bankJoltage << std::endl;
        joltageSum += bankJoltage;
    }

    file.close();
    return joltageSum;
}

int main()
{
    auto maxJoltage = processInputFile("input/input.txt");

    std::cout << "Max joltage: " << maxJoltage << std::endl;
    return 0;
}
