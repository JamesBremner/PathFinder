#include <algorithm>
#include <iostream>
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
    std::reverse( ret.begin(), ret.end() );

    return ret;
}
cGraphData
spanningTree(
    const cGraphData &g,
    const std::string &startName)
    {
        cGraphData spanTree;

        int start = g.find( startName );

        // track visited vertices
        std::vector<bool> visited(g.vertexCount(), false);

        // add initial arbitrary link
        int v = start;
        auto va = g.adjacentOut(v);
        if (!va.size())
            throw std::runtime_error(
                "spanning tree start vertex unconnected");
        auto w = va[0];
        spanTree.findorAdd(g.userName(v),g.userName(w),"1");
        visited[v] = true;
        visited[w] = true;

        // while nodes remain outside of span
        while (g.vertexCount() > spanTree.vertexCount())
        {
            double min_cost = INT_MAX;
            std::pair<int,int> bestLink;

            // loop over nodes in span
            for (int kv = 0; kv < g.vertexCount(); kv++)
            {
                if (!visited[kv])
                    continue;
                v = kv;

                // loop over adjacent nodes not in span
                for (auto w : g.adjacentOut(v))
                {
                    if (visited[w])
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