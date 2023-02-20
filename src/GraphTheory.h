#pragma once
#include <functional>
#include "cGraphData.h"

namespace raven
{

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

        std::vector<std::pair<int, int>>
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
        /// @return -1 no jump required
        /// @return -2 no unvisited leaves

        int isLeafJump(int v);

        std::vector<int> vectorgraphIndexFromSpanIndex(
            const std::vector<int> &visp);
    };

    /// @brief find shortest path from start node to every other
    /// @param g
    /// @param startName
    /// @return pair: vector of preceeding node index for every node, vector of cost of path from start to vertex

    std::pair<std::vector<int>, std::vector<double>>
    dijsktra(
        const cGraphData &g,
        const std::string &startName);
    std::pair<std::vector<int>, std::vector<double>>
    dijsktra(
        const cGraphData &g,
        int start);

    /// @brief find shortest path from start to end node
    /// @param g
    /// @param startName
    /// @param endName
    /// @return pair: vector of node indices on the path, path cost
    /// @return pair: empty vector, -1 when end is not reachable from start

    std::pair<std::vector<int>, double>
    path(
        const cGraphData &g,
        const std::string &startName,
        const std::string &endName);

    std::pair<std::vector<int>, double>
    path(
        const cGraphData &g,
        int start,
        int end);

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

    /// @brief cycle finder
    /// @param g
    /// @param start
    /// @return vector of cycles

    std::vector<std::vector<int>>
    dfs_cycle_finder(
        const cGraphData &g,
        const std::string &start);

    /// @brief path visiting every node

    std::vector<int>
    tourNodes(
        const cGraphData &g);

    void cliques(
        const cGraphData &g,
        std::string& results );

    /// @brief Graph description in graphviz dot format
    /// @param g
    /// @param vpath
    /// @param all
    /// @return string in graphviz dot format

    std::string pathViz(
        cGraphData &g,
        const std::vector<int> &vpath,
        bool all);

    /// @brief Run the graphviz dot layout program
    /// @param[in] g
    /// @param[in] pathViz string in graphviz dot format

    void RunDOT(
        cGraphData &g,
        const std::string &pathViz);
}
