
#include <algorithm>
#include <iostream>
#include <stack>
#include "GraphTheory.h"

namespace raven { namespace graph {

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
        if (g.find( v, f ) != -1 )
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
    //std::cout << "add " << spanTree.userName(v) << "\n";
    if (spanVisited[v])
    {
        if (std::find(revisited.begin(), revisited.end(), v) == revisited.end())
            revisited.push_back(v);
    }
    spanVisited[v] = true;
    tour.push_back(v);
}

void cTourNodes::calculate()
{
    auto best = tour;
    int bestUnvisited = INT_MAX;
    int bestRevisited = INT_MAX;

    // loop over nodes, starting the spanning tree at each
    for (int spanTreeRoot = 0; spanTreeRoot < g.vertexCount(); spanTreeRoot++)
    {
        // find a spanning tree
        spanTree = spanningTree(
            g,
            g.userName(spanTreeRoot));
        if (!spanTree.vertexCount())
            continue;
        // std::cout << "===== trying span from " <<  g.userName(spanTreeRoot) << "=========\n"
        //     << spanTree.text()
        //     << "=================\n";

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
                // TODO: implement path to nearest unvisited vertex
                break;
            }
            if (unvisited == prevUnvisited)
                break;
            prevUnvisited = unvisited;

            // depth first search
            dfs(
                spanTree,
                g.userName(dfsStart),
                std::bind(
                    cTourNodes::visitor, this,
                    std::placeholders::_1));

        }
        std::cout << "tourNodes revisited " << revisited.size()
                  << " unvisited " << unvisited
                  << " start " << g.userName(spanTreeRoot) << "\n";

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
            bestRevisited = revisited.size();
            improve = true;
        }
        else if (unvisited == bestUnvisited)
        {
            if (revisited.size() < bestRevisited)
            {
                best = vectorgraphIndexFromSpanIndex(tour);
                bestUnvisited = unvisited;
                bestRevisited = revisited.size();
                improve = true;
            }
        }
        tour.clear();

        // if (improve)
        // {
        //     std::cout << "improved ";
        //     for (int isp : best)
        //         std::cout << g.userName(isp) << ' ';
        //     std::cout << "\n\n";
        // }
    }

    tour = best;
}

std::vector<int> cTourNodes::vectorgraphIndexFromSpanIndex(const std::vector<int> &visp)
{
    std::vector<int> ret;
    for (int isp : visp)
        ret.push_back(g.find(spanTree.userName(isp)));
    return ret;
}

std::vector<std::pair<int, int>>
cTourNodes::spanTree_get() const
{
    return spanTree.edgeList();
}
}
}