#pragma once
#include <functional>
#include "cGraphData.h"

class cTourNodes
{
public:
    cTourNodes(const cGraphData &theGraph)
        : g(theGraph)
    {
    }

    void calculate();

    std::vector<int> getTour() const
    {
        return tour;
    }

    std::vector<std::pair<int,int>>
        spanTree_get() const;


private:
    const cGraphData &g;
    cGraphData spanTree;

    int dfsStart;
    int unvisited;
    std::vector<int> vleaf;
    std::vector<bool> spanVisited;
    std::vector<int> revisited;

    std::vector<int> tour;

    void tourNodesAdd(int v);

    bool visitor(int v);

    /// @brief find leaf to jump to from leaf
    /// @param v 
    /// @return >= 0 node index to jump to
    /// @return -1 v is not a leaf
    /// @return -2 no unvisited leaves

    int isLeafJump(int v );
};

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
/// @return empty vector when end is not reachable from start

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