
#include <algorithm>
#include <iostream>
#include <stack>
#include "GraphTheory.h"

namespace raven
{
    namespace graph
    {

        bool cTourNodes::visitor(
            int v)
        {
            // add node to tour
            tourNodesAdd(v);

            // stop if all nodes visited
            unvisited = std::count(
                spanVisited.begin(),
                spanVisited.end(),
                false);
            if (!unvisited)
                return false;

            dfsStart = isLeafJump(v);
            if (dfsStart == -1)
                return true;
            if (dfsStart == -2)
                return false;
            if (dfsStart == -3)
                return false;

            // std::cout << "jump " << spanTree.userName(v)
            //           << " to " << spanTree.userName(dfsStart) << "\n";

            // start a new dfs starting from the unvisited leaf
            return false;
        }

        int cTourNodes::isLeafJump(int v)
        {
            // check if at a leaf
            if (std::find(
                    vleaf.begin(),
                    vleaf.end(),
                    v) == vleaf.end())
                return -1;

            // reached a leaf of the spanning tree
            // check if we reached this leaf by jumping
            if (!spanVisited[spanTree.adjacentOut(v)[0]])
                return -1;

            // jump to an unvisted leaf
            for (int f : vleaf)
            {
                if (f == v)
                    continue;

                // check other leaf unvisited
                if (spanVisited[f])
                    continue;

                // check for link to other leaf in original graph
                if (g->find(v, f) != -1)
                    return f;

                // there is no escape from this leaf without revisiting nodes

                return -3;
            }

            // no unvisited leaves
            return -2;
        }

        void cTourNodes::tourNodesAdd(
            int v)
        {
            // std::cout << "add " << spanTree.userName(v) << "\n";
            if (spanVisited[v])
            {
                if (std::find(revisited.begin(), revisited.end(), v) == revisited.end())
                    revisited.push_back(v);
            }
            spanVisited[v] = true;
            tour.push_back(v);
        }

        void cTourNodes::calculate( sGraphData &gd)
        {
            if( ! gd.g.vertexCount()) {
                std::cout << "cTourNodes::calculate no vertices\n";
                return;
            }

            g = &gd.g;
            myEdgeWeights = gd.edgeWeight;

            auto best = tour;
            int bestUnvisited = INT_MAX;
            std::vector<int>bestRevisited;

            // loop over nodes, starting the spanning tree at each
            for (int spanTreeRoot = 0; spanTreeRoot < g->vertexCount(); spanTreeRoot++)
            {
                // find a spanning tree
                // note: spanTree.userName( vi ) == g.userName( vi )

                gd.startName = gd.g.userName( spanTreeRoot );
                spanTree = spanningTree( gd );
                if (!spanTree.vertexCount())
                    continue;

                // find spanning tree leaves
                for (int v = 0; v < spanTree.vertexCount(); v++)
                {
                    if (spanTree.adjacentOut(v).size() == 1)
                        vleaf.push_back(v);
                }

                spanVisited.clear();
                spanVisited.resize(spanTree.vertexCount(), false);
                revisited.clear();
                dfsStart = spanTreeRoot;
                unvisited = spanTree.vertexCount();
                int prevUnvisited = unvisited + 1;

                // while unvisited nodes remain
                while (unvisited)
                {
                    // check on progress
                    if (dfsStart == -2)
                        break;
                    if (dfsStart == -3)
                    {
                        // stuck on a leaf with no one hop reachable unvisited nodes
                        dfsStart = PathToUnvisited();
                        if (dfsStart < 0)
                            break;
                    }
                    if (unvisited == prevUnvisited)
                        break;
                    prevUnvisited = unvisited;

                    // depth first search
                    dfs(
                        spanTree,
                        dfsStart,
                        std::bind(
                            cTourNodes::visitor, this,
                            std::placeholders::_1));
                }
                // std::cout << "tourNodes revisited " << revisited.size()
                //           << " unvisited " << unvisited
                //           << " start " << g->userName(spanTreeRoot)
                //           << " ( " << spanTreeRoot << " )"
                //           << "\n";

                // check for 'perfect' tour
                if ((!revisited.size()) && (!unvisited))
                {
                    tour = vectorgraphIndexFromSpanIndex(tour);
                    return;
                }

                bool improve = false;
                if (unvisited < bestUnvisited)
                {
                    best = vectorgraphIndexFromSpanIndex(tour);
                    bestUnvisited = unvisited;
                    bestRevisited = revisited;
                    improve = true;
                }
                else if (unvisited == bestUnvisited)
                {
                    if (revisited.size() < bestRevisited.size())
                    {
                        best = vectorgraphIndexFromSpanIndex(tour);
                        bestUnvisited = unvisited;
                        bestRevisited = revisited;
                        improve = true;
                    }
                }
                tour.clear();
            }

            std::cout << "bestPath ";
            for (int isp : best)
                std::cout << g->userName(isp) << ' ';
            std::cout << "\n\n";

            tour = best;
            unvisited = bestUnvisited;
            revisited = bestRevisited;
        }

        int cTourNodes::PathToUnvisited()
        {
            int bestHops = INT_MAX;
            std::vector<int> bestPath;

            // loop over nodes
            for (int target = 0; target < g->vertexCount(); target++)
            {
                // check for visited
                if (spanVisited[spanTree.find(g->userName(target))])
                    continue;

                // find best path from last node in tour
                // allowing node revisits
                sGraphData gd;
                gd.g = spanTree;
                gd.edgeWeight = myEdgeWeights;
                gd.startName = g->userName(tour.back());
                gd.endName = g->userName(target);
                auto pathret = path( gd );

                // check for a shorter path
                if (pathret.first.size() && pathret.first.size() < bestHops)
                {
                    bestHops = pathret.first.size();
                    bestPath = pathret.first;
                }
            }
            if (!bestPath.size())
                return -1;

            // add path to tour, except for first and last node
            for (
                auto it = bestPath.begin() + 1;
                it != bestPath.end() - 1;
                it++)
                tourNodesAdd(*it);

            // return last node in path
            // the next DFS will start here
            return bestPath.back();
        }

        std::vector<int> cTourNodes::vectorgraphIndexFromSpanIndex(const std::vector<int> &visp)
        {
            std::vector<int> ret;
            for (int isp : visp)
                ret.push_back(g->find(spanTree.userName(isp)));
            return ret;
        }

        std::vector<std::pair<int, int>>
        cTourNodes::spanTree_get() const
        {
            return spanTree.edgeList();
        }
    }
}