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
    int x;
    int y;
    int z;
};

class Distance
{
public:
    double distance;
    std::pair<size_t, size_t> pointIndices;
    Coordinates point1;
    Coordinates point2;

    bool operator<(const Distance &other) const
    {
        return distance < other.distance;
    }
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
        size_t secondComma = line.find(',', firstComma + 1);
        if (firstComma == std::string::npos || secondComma == std::string::npos)
            continue;

        int x = std::stoi(line.substr(0, firstComma));
        int y = std::stoi(line.substr(firstComma + 1, secondComma - firstComma - 1));
        int z = std::stoi(line.substr(secondComma + 1));
        data.push_back({x, y, z});
    }

    file.close();
    return data;
}

std::vector<Distance> calculateDistances(const std::vector<Coordinates> &points)
{
    std::vector<Distance> distances;
    for (size_t i = 0; i < points.size(); i++)
    {
        for (size_t j = i + 1; j < points.size(); j++)
        {
            double dist = std::sqrt(std::pow(points[i].x - points[j].x, 2) +
                                    std::pow(points[i].y - points[j].y, 2) +
                                    std::pow(points[i].z - points[j].z, 2));
            distances.push_back({dist, {i, j}, points[i], points[j]});
        }
    }

    std::sort(distances.begin(), distances.end());

    return distances;
}

using CircuitNodes = std::vector<std::vector<size_t>>;
using NodeToCircuitMapping = std::unordered_map<size_t, size_t>; // Maps node to circuit index

void connectTwoNodes(NodeToCircuitMapping &nodeToCircuitMapping, CircuitNodes &circuitNodes, size_t node1, size_t node2)
{
    // std::cout << "Evaluating the connection of nodes " << node1 << " and " << node2 << std::endl;
    size_t targetCircuitIndex;
    auto node1Mapping = nodeToCircuitMapping.find(node1);
    auto node2Mapping = nodeToCircuitMapping.find(node2);

    if (node1Mapping != nodeToCircuitMapping.end() && node2Mapping != nodeToCircuitMapping.end())
    {
        size_t circuitIndex1 = node1Mapping->second;
        size_t circuitIndex2 = node2Mapping->second;

        if (circuitIndex1 == circuitIndex2)
        {
            // std::cout << "Both nodes are already in the same circuit." << std::endl;
            return; // already in the same circuit
        }
    }
    if (node1Mapping != nodeToCircuitMapping.end())
    {
        targetCircuitIndex = node1Mapping->second;
    }
    else if (node2Mapping != nodeToCircuitMapping.end())
    {
        targetCircuitIndex = node2Mapping->second;
    }
    else
    {
        // std::cout << "Connecting nodes " << node1 << " and " << node2 << " into a new circuit." << std::endl;
        // create new circuit
        circuitNodes.push_back({node1, node2});
        targetCircuitIndex = circuitNodes.size() - 1;
        nodeToCircuitMapping[node1] = targetCircuitIndex;
        nodeToCircuitMapping[node2] = targetCircuitIndex;
        return;
    }

    auto addNodeToCircuit = [&](size_t node) -> void
    {
        auto nodeMapping = nodeToCircuitMapping.find(node);
        if (nodeMapping == nodeToCircuitMapping.end())
        {
            // node is not yet in a circuit, just add it
            // std::cout << "Adding node " << node << " to existing circuit " << targetCircuitIndex << std::endl;
            circuitNodes[targetCircuitIndex].push_back(node);
            nodeToCircuitMapping[node] = targetCircuitIndex;
        }
        else
        {
            // std::cout << "Merging circuit of node " << node << " into circuit " << targetCircuitIndex << std::endl;
            // node is already in a circuit, need to merge circuits
            size_t sourceCircuitIndex = nodeMapping->second;

            if (sourceCircuitIndex == targetCircuitIndex)
                return;

            // Merge source circuit into target circuit
            for (const auto &n : circuitNodes[sourceCircuitIndex])
            {
                circuitNodes[targetCircuitIndex].push_back(n);
                nodeToCircuitMapping[n] = targetCircuitIndex;
            }
            // Clear the source circuit (don't erase to avoid invalidating indices)
            circuitNodes[sourceCircuitIndex].clear();
        }
    };

    addNodeToCircuit(node1);
    addNodeToCircuit(node2);

    return;
}

size_t countNodesInSingleCircuit(const CircuitNodes &circuitNodes)
{
    for (const auto &circuit : circuitNodes)
    {
        if (!circuit.empty())
            return circuit.size();
    }
    return 0U;
}

long long connectJunctionBoxes(const std::vector<Coordinates> &inputData, const std::vector<Distance> &distances, bool part1 = true)
{
    std::vector<std::vector<size_t>> circuitNodes;
    std::unordered_map<size_t, size_t> nodeToCircuitMapping; // Map node to circuit index

    size_t connectedNodes = 0U;
    for (auto const &distance : distances)
    {
        connectTwoNodes(nodeToCircuitMapping, circuitNodes,
                        distance.pointIndices.first,
                        distance.pointIndices.second);

        connectedNodes++;
        if (part1)
        {
            if (connectedNodes >= 1000)
                break;
        }
        else
        {
            if (countNodesInSingleCircuit(circuitNodes) == inputData.size())
            {
                std::cout << "All node connected after processing " << connectedNodes << " connections." << std::endl;
                std::cout << "Last connection was between points (" << distance.point1.x << "," << distance.point1.y << "," << distance.point1.z << ") and ("
                          << distance.point2.x << "," << distance.point2.y << "," << distance.point2.z << ")" << std::endl;
                return static_cast<long long>(distance.point1.x) * distance.point2.x;
            }
        }
    }

    // only part 1
    std::vector<size_t> circuitSizes;
    for (const auto &circuit : circuitNodes)
    {
        if (!circuit.empty())
        {
            circuitSizes.push_back(circuit.size());
        }
    }

    std::sort(circuitSizes.begin(), circuitSizes.end(), std::greater<size_t>());

    // Caclulate the multiplication of the sizes of the three largest circuits
    size_t result = 1U;
    for (size_t i = 0U; i < std::min(size_t(3U), circuitSizes.size()); i++)
    {
        result *= circuitSizes[i];
    }
    return result;
}

int main()
{
    auto inputData = readInputFile("input/input.txt");
    auto distances = calculateDistances(inputData);
    size_t numCircuits = connectJunctionBoxes(inputData, distances, false);
    std::cout << "Result: " << numCircuits << std::endl;
    return 0;
}
