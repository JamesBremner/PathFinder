#pragma once
#include <set>
#include <functional>
#include "cGraph.h"

namespace raven
{
    namespace graph
    {

        class cTourNodes
        {
        public:
            // cTourNodes(const cGraph &theGraph)
            //     : g(theGraph)
            // {
            // }

            void calculate(const cGraph &g);

            std::vector<int> getTour() const
            {
                return tour;
            }

            std::vector<std::pair<int, int>>
            spanTree_get() const;

            int unvisitedCount() const
            {
                return unvisited;
            }
            int revisitedCount() const
            {
                return revisited.size();
            }

        private:
            const cGraph *g;
            cGraph spanTree;

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

            int PathToUnvisited();

            std::vector<int> vectorgraphIndexFromSpanIndex(
                const std::vector<int> &visp);
        };

        class cSpanningTree
        {
        public:
            cGraph mySpanningTree;
            std::set<int> myVertexSet;

            void add(
                const cGraph &g,
                int v, int w);

            int vertexCount() const
            {
                return myVertexSet.size();
            }
        };

        /// @brief find shortest path from start node to every other
        /// @param g
        /// @param start vertex index
        /// @param[out] dist shortest distance from start to each node
        /// @param[out] pred previous node on shortest path to each node

        void dijsktra(
            const cGraph &g,
            int start,
            std::vector<double> &dist,
            std::vector<int> &pred);

        /// @brief find shortest path from start to end node
        /// @param g
        /// @param startName
        /// @param endName
        /// @return pair: vector of node indices on the path, path cost
        /// @return pair: empty vector, -1 when end is not reachable from start

        std::pair<std::vector<int>, double>
        path(
            const cGraph &g,
            const std::string &startName,
            const std::string &endName);

        /// @brief find all paths between two nodes
        /// @param g
        /// @param start
        /// @param end
        /// @return vector of vectors of node indices on paths

        std::vector<std::vector<int>>
        allPaths(
            const cGraph &g,
            int start,
            int end);

        std::pair<std::vector<int>, double>
        path(
            const cGraph &g,
            int start,
            int end);

        /// @brief find spanning tree
        /// @param g
        /// @param startName root node
        /// @return graph - a tree rooted at start and visiting every node

        cGraph
        spanningTree(
            const cGraph &g,
            const std::string &startName);

        /// @brief depth first search
        /// @param g
        /// @param startIndex
        /// @param visitor function to call when a new node is reached
        /// visitor should return true, but false if the search should stop

        void dfs(
            const cGraph &g,
            int startIndex,
            std::function<bool(int v)> visitor);

        /// @brief cycle finder
        /// @param g
        /// @param startIndex index of vertex to be included in cycles, default to -1 if all cycles required
        /// @return vector of cycles

        std::vector<std::vector<int>>
        dfs_cycle_finder(
            const cGraph &g,
            int startIndex = -1);

        /// @brief find all paths between 2 vertices
        /// @param g 
        /// @param startIndex 
        /// @param destIndex 
        /// @return vector of path vectors
        
        std::vector<std::vector<int>>
        dfs_allpaths(
            const cGraph &g,
            int startIndex,
            int destIndex);

        /// @brief path visiting every node

        std::vector<int>
        tourNodes(
            const cGraph &g);

        void cliques(
            const cGraph &g,
            std::string &results);

        /// @brief Maximum flow between two vertices
        /// @param g
        /// @param start
        /// @param end
        /// @param[out] vEdgeFlow flow through each edge
        /// @return total flow

        double flows(
            const cGraph &g,
            int start,
            int end,
            std::vector<int>& vEdgeFlow );

        /// @brief Maximum flow through graph with multiple sources
        /// @param g
        /// @param vsource vector of source vertex indices
        /// @param end sink vertex index
        /// @return maximum flow

        double multiflows(
            const cGraph &g,
            const std::vector<int> &vsource,
            int end);

        /// @brief Find connected source and sinks
        /// @param g the graph
        /// @return A vector of vectors containing a source index and the connected sink indices
        ///
        /// A source has a zero in-degree, a sink has a zero out-degree

        std::vector<std::vector<int>> sourceToSink(
            const cGraph &g);

        double probs(cGraph &g, int end);

        std::vector<std::string> alloc(cGraph& g );

        /// @brief  Find euler path through graph
        /// @param g graph
        /// @return vector of vertex indices on path
        /// If no euler path, exception thrown
        
        std::vector<int> euler( const cGraph& g);

        /// @brief find set of vertices that cover every link
        /// @param g 
        /// @return vector of vertex indices
        std::vector<int>
        vertexCover(const cGraph &g);

        /// @brief Graph description in graphviz dot format
        /// @param g
        /// @param vpath
        /// @param all
        /// @return string in graphviz dot format

        std::string pathViz(
            cGraph &g,
            const std::vector<int> &vpath,
            bool all);

        /// @brief Run the graphviz dot layout program
        /// @param[in] g
        /// @param[in] pathViz string in graphviz dot format

        void RunDOT(
            cGraph &g,
            const std::string &pathViz);
    }
}
