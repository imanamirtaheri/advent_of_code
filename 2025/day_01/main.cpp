#include <fstream>
#include <iostream>
#include <string>

constexpr int INITIAL_DIAL_POSITION = 50;

enum class EvaluationStrategy : uint8_t
{
    ONLY_LANDING,        // Part 1
    CROSSING_AND_LANDING // Part 2
};

int parseRotationValue(std::string const &line)
{
    char direction = line[0];
    int rotationValue = std::stoi(line.substr(1));

    if (direction == 'L')
    {
        rotationValue = -rotationValue;
    }
    return rotationValue;
}

// processes the rotation, calculate the new dial position and number of times
// it hits position 0 according to the evaluation method
std::pair<int, int> processRotation(int currentPosition, int rotationValue, EvaluationStrategy method)
{
    int nextPosition = currentPosition + rotationValue;
    int correctedNextPosition = (nextPosition % 100 + 100) % 100; // wrap around 0-99;
    int zeroHits = 0;

    if (method == EvaluationStrategy::ONLY_LANDING)
    {
        if (correctedNextPosition == 0)
            zeroHits = 1;
    }
    else if (method == EvaluationStrategy::CROSSING_AND_LANDING)
    {
        auto mapToCycle = [](int pos) -> int
        {
            return pos >= 0 ? (pos / 100) : (pos / 100) - 1;
        };

        if (rotationValue >= 0)
        {
            // example: 50 -> 150
            zeroHits = mapToCycle(nextPosition);
        }
        else
        {
            // example: 50 -> -60
            zeroHits = mapToCycle(currentPosition - 1) - mapToCycle(nextPosition);

            if (nextPosition == 0)
            {
                // Special case: landing exactly on 0 when going negative
                zeroHits += 1;
            }
        }
    }
    // std::cout << "From " << currentPosition << " to " << correctedNextPosition << " [" << nextPosition << "] crosses 0 for " << zeroHits << " times." << std::endl;
    return {correctedNextPosition, zeroHits};
}
int processInput(const std::string &filename, EvaluationStrategy method)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 0;
    }

    int dialPosition = INITIAL_DIAL_POSITION;
    int totalZeroHits = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        int rotationValue = parseRotationValue(line);
        auto [newPosition, zeroHits] = processRotation(dialPosition, rotationValue, method);
        dialPosition = newPosition;
        totalZeroHits += zeroHits;
    }

    file.close();
    return totalZeroHits;
}

int main()
{
    std::string inputFile = "input/input.txt";
    int part1Hits = processInput(inputFile, EvaluationStrategy::ONLY_LANDING);
    std::cout << "Part 1: Number of times the dial was at position 0: " << part1Hits << std::endl;

    int part2Hits = processInput(inputFile, EvaluationStrategy::CROSSING_AND_LANDING);
    std::cout << "Part 2: Number of times the dial was at position 0: " << part2Hits << std::endl;
    return 0;
}
