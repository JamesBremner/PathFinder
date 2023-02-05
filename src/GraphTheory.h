#include <functional>
#include "cGraphData.h"

/// @brief find shortest path from start node to every other
/// @param g 
/// @param startName 
/// @return vector of preceeding node index for every node

std::vector<int>
dijsktra(
    const cGraphData &g,
    const std::string &startName);

/// @brief find shortest path from start to end node
/// @param g 
/// @param startName 
/// @param endName 
/// @return vector of node indices on the path

std::vector<int>
path(
    const cGraphData &g,
    const std::string &startName,
    const std::string &endName);

/// @brief find spanning tree 
/// @param g 
/// @param startName root node
/// @return graph - a tree rooted at start and visiting every node

cGraphData
spanningTree(
    const cGraphData &g,
    const std::string &startName);

/// @brief depth first search
/// @param g 
/// @param startName 
/// @param visitor function to call when a new node is reached
/// visitor should return true, but false if the search should stop

void dfs(
    const cGraphData &g,
    const std::string &startName,
    std::function<bool(int v)> visitor);

/// @brief path visiting every node
std::vector<int>
tourNodes(
    const cGraphData &g );