#include <algorithm>
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