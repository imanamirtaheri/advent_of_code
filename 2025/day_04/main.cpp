#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using PaperGridType = std::vector<std::vector<bool>>;
using AdjacentGridType = std::vector<std::vector<int>>;
constexpr int PAPER_ACCESS_THRESHOLD = 4;
constexpr bool IS_PART_2 = true;

PaperGridType readInputFile(std::string const &filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return {};
    }

    PaperGridType grid;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::vector<bool> row;
        row.reserve(line.size());
        for (char c : line)
        {
            row.push_back(c == '@');
        }
        grid.push_back(row);
    }

    return grid;
}

AdjacentGridType computeAdjacentGrid(const PaperGridType &paperGrid)
{
    int numRows = paperGrid.size();
    int numCols = paperGrid.empty() ? 0 : paperGrid[0].size();
    AdjacentGridType adjacentGrid(numRows, std::vector<int>(numCols, 0));

    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            if (paperGrid[r][c])
            {
                // Increment adjacent cells
                for (int dr = -1; dr <= 1; ++dr)
                {
                    for (int dc = -1; dc <= 1; ++dc)
                    {
                        if (dr == 0 && dc == 0)
                            continue; // skip the cell itself

                        int nr = r + dr;
                        int nc = c + dc;

                        if (nr < numRows && nc < numCols && nr >= 0 && nc >= 0)
                        {
                            adjacentGrid[nr][nc]++;
                        }
                    }
                }
            }
        }
    }

    return adjacentGrid;
}

int countRemovablePapers(const PaperGridType &paperGrid, const AdjacentGridType &adjacentGrid)
{
    int count = 0;
    int numRows = paperGrid.size();
    int numCols = paperGrid.empty() ? 0 : paperGrid[0].size();
    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            if (paperGrid[r][c] && adjacentGrid[r][c] < PAPER_ACCESS_THRESHOLD)
            {
                std::cout << 'x';
                count++;
            }
            else
            {
                std::cout << (paperGrid[r][c] ? '@' : '.');
            }
        }
        std::cout << std::endl;
    }
    return count;
}

int removePapersRecursively(PaperGridType &paperGrid, AdjacentGridType &adjacentGrid)
{
    int removedPapers = 0;
    int numRows = paperGrid.size();
    int numCols = paperGrid.empty() ? 0 : paperGrid[0].size();
    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            if (paperGrid[r][c] && adjacentGrid[r][c] < PAPER_ACCESS_THRESHOLD)
            {
                paperGrid[r][c] = false;
                removedPapers++;

                // Update adjacent grid
                for (int dr = -1; dr <= 1; ++dr)
                {
                    for (int dc = -1; dc <= 1; ++dc)
                    {
                        if (dr == 0 && dc == 0)
                            continue; // skip the cell itself
                        int nr = r + dr;
                        int nc = c + dc;
                        if (nr < numRows && nc < numCols && nr >= 0 && nc >= 0)
                        {
                            adjacentGrid[nr][nc]--;
                        }
                    }
                }
            }
        }
    }

    if (removedPapers)
    {
        // Recursively remove more accessible papers
        removedPapers += removePapersRecursively(paperGrid, adjacentGrid);
    }
    return removedPapers;
}
int main()
{
    auto grid = readInputFile("input/sample.txt");
    auto adjacentGrid = computeAdjacentGrid(grid);

    if (IS_PART_2)
    {
        int removedCount = removePapersRecursively(grid, adjacentGrid);
        std::cout << "Number of removed papers: " << removedCount << std::endl;
    }
    else
    {
        int removableCount = countRemovablePapers(grid, adjacentGrid);
        std::cout << "Number of removable papers: " << removableCount << std::endl;
    }
    return 0;
}
