#include <algorithm>
#include <iostream>
#include <stack>
#include "GraphTheory.h"

std::vector<int>
dijsktra(
    const cGraphData &g,
    const std::string &startName)
{
    // shortest distance from start to each node
    std::vector<double> dist(g.vertexCount(), INT_MAX);

    // previous node on shortest path to each node
    std::vector<int> pred(g.vertexCount(), -1);

    std::vector<bool> sptSet(g.vertexCount(), false); // sptSet[i] will be true if vertex i is included in shortest
                                                      // path tree or shortest distance from src to i is finalized

    int start = g.find(startName);

    // Distance of source vertex from itself is always 0
    dist[start] = 0;
    pred[start] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < g.vertexCount() - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int min = INT_MAX, uidx;
        for (int vidx = 0; vidx < g.vertexCount(); vidx++)
            if (sptSet[vidx] == false && dist[vidx] <= min)
            {
                min = dist[vidx];
                uidx = vidx;
            }
        if (min == INT_MAX)
        {
            // no more nodes connected to start
            break;
        }

        // Mark the picked vertex as processed
        sptSet[uidx] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (auto vp : g.adjacentOut(uidx))
        {
            if (sptSet[vp])
                continue; // already processed

            // Update dist[v] only if total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            double cost = g.edgeAttr(uidx, vp, 0);
            if (dist[uidx] + cost < dist[vp])
            {
                dist[vp] = dist[uidx] + cost;
                pred[vp] = uidx;
            }
        }
    }
    return pred;
}

std::vector<int>
path(
    const cGraphData &g,
    const std::string &startName,
    const std::string &endName)
{
    auto pred = dijsktra(g, startName);

    std::vector<int> ret;

    int start = g.find(startName);
    int end = g.find(endName);

    ret.push_back(end);
    int next = end;
    while (1)
    {
        next = pred[next];
        ret.push_back(next);
        if (next == start)
            break;
    }
    std::reverse(ret.begin(), ret.end());

    return ret;
}
cGraphData
spanningTree(
    const cGraphData &g,
    const std::string &startName)
{
    cGraphData spanTree;

    int start = g.find(startName);

    // track visited vertices
    std::vector<bool> visited(g.vertexCount(), false);

    // add initial arbitrary link
    int v = start;
    auto va = g.adjacentOut(v);
    if (!va.size())
        throw std::runtime_error(
            "spanning tree start vertex unconnected");
    auto w = va[0];
    spanTree.findorAdd(g.userName(v), g.userName(w), "1");
    visited[v] = true;
    visited[w] = true;

    // while nodes remain outside of span
    while (g.vertexCount() > spanTree.vertexCount())
    {
        double min_cost = INT_MAX;
        std::pair<int, int> bestLink;

        // loop over nodes in span
        for (int kv = 0; kv < g.vertexCount(); kv++)
        {
            if (!visited[kv])
                continue;
            v = kv;

            // loop over adjacent nodes not in span
            auto dbg = g.adjacentOut(v);
            for (auto w : g.adjacentOut(v))
            {
                if (visited[w])
                    continue;
                if (v > w)
                    continue;

                double cost = g.edgeAttr(v, w, 0);
                if (cost > 0)
                {
                    if (cost < min_cost)
                    {
                        min_cost = cost;
                        bestLink = std::make_pair(v, w);
                    }
                }
            }
        }

        // add cheapest link between node in tree to node not yet in tree
        spanTree.findorAdd(
            g.userName(bestLink.first),
            g.userName(bestLink.second),
            "1");
        visited[bestLink.first] = true;
        visited[bestLink.second] = true;
    }

    return spanTree;
}

void dfs(
    const cGraphData &g,
    const std::string &startName,
    std::function<void(int v)> visitor)
{
    // track visited vertices
    std::vector<bool> visited(g.vertexCount(), false);

    // vertices waiting to be visited
    std::stack<int> wait;

    /*  1 Start by putting one of the graph's vertices on top of a stack.
        2 Take the top vertex of the stack and add it to the visited list.
        3 Add adjacent vertices which aren't in the visited list to the top of the stack.
        4 Keep repeating steps 2 and 3 until the stack is empty.
    */

    wait.push(g.find(startName));

    while (!wait.empty())
    {
        int v = wait.top();
        wait.pop();
        if (visited[v])
            continue;
        visitor(v);
        visited[v] = true;

        for (int w : g.adjacentOut(v))
            if (!visited[w])
                wait.push(w);
    }
}

std::vector<int>
tourNodes(
    const cGraphData &g)
{
    // find a spanning tree
    auto spanTree = spanningTree(
        g,
        g.userName(0));
    if (!spanTree.vertexCount())
        throw std::runtime_error(
            "tourNodes cannot find spanning tree");

    std::vector<int> tour;
    tour.push_back(0);

    // find spanning tree leaves
    std::vector<int> vLeaves;
    for (int vi = 0; vi < spanTree.vertexCount(); vi++)
    {
        if (!spanTree.adjacentOut(vi).size())
        {
            vLeaves.push_back(vi);
        }
    }

    for (int leaf1 : vLeaves)
    {
        for (int leaf2 : vLeaves)
        {
            if (leaf1 == leaf2)
                continue;
        }
    }
}