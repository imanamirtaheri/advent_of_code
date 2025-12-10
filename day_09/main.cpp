#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

class Coordinates
{
public:
    uint32_t x;
    uint32_t y;
};

std::vector<Coordinates>
readInputFile(std::string const &filePath)
{
    std::vector<Coordinates> data;
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

        // parse coordinates in format x,y,z
        size_t firstComma = line.find(',');
        if (firstComma == std::string::npos)
            continue;

        uint32_t x = std::stoul(line.substr(0, firstComma));
        uint32_t y = std::stoul(line.substr(firstComma + 1));
        data.push_back({x, y});
    }

    file.close();
    return data;
}

// Check if point is inside or on the boundary of the polygon using ray casting
bool isInsideOrOnPolygon(const Coordinates &point, const std::vector<Coordinates> &polygon)
{
    int n = polygon.size();
    bool inside = false;

    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        uint32_t xi = polygon[i].x, yi = polygon[i].y;
        uint32_t xj = polygon[j].x, yj = polygon[j].y;

        // Check if point is on the edge (horizontal or vertical line segment)
        if ((xi == xj && point.x == xi && point.y >= std::min(yi, yj) && point.y <= std::max(yi, yj)) ||
            (yi == yj && point.y == yi && point.x >= std::min(xi, xj) && point.x <= std::max(xi, xj)))
        {
            return true; // On boundary
        }

        // Ray casting algorithm
        if (((yi > point.y) != (yj > point.y)) &&
            (point.x < (int64_t)(xj - xi) * (int64_t)(point.y - yi) / (int64_t)(yj - yi) + xi))
        {
            inside = !inside;
        }
    }

    return inside;
}

// Check if entire rectangle is inside or on the polygon
bool isRectangleValid(const Coordinates &c1, const Coordinates &c2, const std::vector<Coordinates> &polygon)
{
    uint32_t minX = std::min(c1.x, c2.x);
    uint32_t maxX = std::max(c1.x, c2.x);
    uint32_t minY = std::min(c1.y, c2.y);
    uint32_t maxY = std::max(c1.y, c2.y);

    // Check all four corners
    Coordinates corners[4] = {
        {minX, minY},
        {minX, maxY},
        {maxX, minY},
        {maxX, maxY}};

    for (int i = 0; i < 4; i++)
    {
        if (!isInsideOrOnPolygon(corners[i], polygon))
            return false;
    }

    // Check points along all edges
    for (uint32_t x = minX; x <= maxX; x++)
    {
        if (!isInsideOrOnPolygon({x, minY}, polygon))
            return false;
        if (!isInsideOrOnPolygon({x, maxY}, polygon))
            return false;
    }

    for (uint32_t y = minY; y <= maxY; y++)
    {
        if (!isInsideOrOnPolygon({minX, y}, polygon))
            return false;
        if (!isInsideOrOnPolygon({maxX, y}, polygon))
            return false;
    }

    return true;
}

std::vector<uint64_t> calculateArea(const std::vector<Coordinates> &points)
{
    size_t numPoints = points.size();
    std::vector<uint64_t> areas;
    areas.reserve(numPoints * (numPoints - 1) / 2);
    for (size_t i = 0; i < points.size(); i++)
    {
        for (size_t j = i + 1; j < points.size(); j++)
        {
            uint64_t area = static_cast<uint64_t>(std::abs(static_cast<int64_t>(points[i].x) - static_cast<int64_t>(points[j].x)) + 1U) *
                            static_cast<uint64_t>(std::abs(static_cast<int64_t>(points[i].y) - static_cast<int64_t>(points[j].y)) + 1U);
            areas.push_back(area);
        }
    }
    return areas;
}

uint64_t calculateMaxAreaWithGreenTiles(const std::vector<Coordinates> &points)
{
    uint64_t maxArea = 0;

    for (size_t i = 0; i < points.size(); i++)
    {
        for (size_t j = i + 1; j < points.size(); j++)
        {
            if (isRectangleValid(points[i], points[j], points))
            {
                uint64_t area = static_cast<uint64_t>(std::abs(static_cast<int64_t>(points[i].x) - static_cast<int64_t>(points[j].x)) + 1U) *
                                static_cast<uint64_t>(std::abs(static_cast<int64_t>(points[i].y) - static_cast<int64_t>(points[j].y)) + 1U);
                maxArea = std::max(maxArea, area);
            }
        }
    }

    return maxArea;
}

int main()
{
    auto inputData = readInputFile("input/input.txt");

    // Part 1
    auto areas = calculateArea(inputData);
    std::cout << "Part 1 - Max area: " << *std::max_element(areas.begin(), areas.end()) << std::endl;

    // Part 2
    uint64_t maxAreaWithGreen = calculateMaxAreaWithGreenTiles(inputData);
    std::cout << "Part 2 - Max area with green tiles: " << maxAreaWithGreen << std::endl;

    return 0;
}
