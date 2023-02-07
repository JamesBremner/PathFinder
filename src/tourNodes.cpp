
#include <algorithm>
#include <iostream>
#include <stack>
#include "GraphTheory.h"

std::vector<int>
tourNodes(
    const cGraphData &g)
{
    cTourNodes T(g);
    T.calculate();
    return T.getTour();
}
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
    if (dfsStart >= 0)
    {
        std::cout << "jump " << spanTree.userName(v)
                  << " to " << spanTree.userName(dfsStart) << "\n";

        // start a new dfs starting from the unvisited leaf
        return false;
    }

    // continue the search
    return true;
}

int cTourNodes::isLeafJump(int v)
{
    if (std::find(
            vleaf.begin(),
            vleaf.end(),
            v) == vleaf.end())
        return -1;

    // reached a leaf of the spanning tree
    // jump to an unvisted leaf
    for (int f : vleaf)
    {
        if (f == v)
            continue;
        if (spanVisited[f])
            continue;

        return f;
    }

    return -1;
}

void cTourNodes::tourNodesAdd(
    int v)
{
    if (spanVisited[v])
    {
        if (std::find(revisited.begin(), revisited.end(), v) == revisited.end())
            revisited.push_back(v);
    }
    spanVisited[v] = true;
    tour.push_back(
        g.find(spanTree.userName(v)));
}

void cTourNodes::calculate()
{
    std::cout << "tourNodes\n";

    for (int spanTreeRoot = 0; spanTreeRoot < g.vertexCount(); spanTreeRoot++)
    {
        // find a spanning tree
        spanTree = spanningTree(
            g,
            g.userName(spanTreeRoot));
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
        dfsStart = 0;
        unvisited = spanTree.vertexCount();
        int prevUnvisited = unvisited + 1;

        // while unvisited nodes remain
        while (unvisited)
        {
            // check on progress
            if (unvisited == prevUnvisited)
            {
                //std::cout << "tourNodes stuck!!!\n";
                break;
            }
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
        if( (! revisited.size() ) && ( ! unvisited ) )
            return;
    }
}