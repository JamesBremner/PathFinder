#include <algorithm>
#include <iostream>
#include <sstream>
#include <stack>
#include "GraphTheory.h"

namespace raven
{

    std::pair<std::vector<int>, std::vector<double>>
    dijsktra(
        const cGraphData &g,
        const std::string &startName)
    {
        return dijsktra(
            g,
            g.find(startName));
    }
    std::pair<std::vector<int>, std::vector<double>>
    dijsktra(
        const cGraphData &g,
        int start)
    {
        // shortest distance from start to each node
        std::vector<double> dist(g.vertexCount(), INT_MAX);

        // previous node on shortest path to each node
        std::vector<int> pred(g.vertexCount(), -1);

        std::vector<bool> sptSet(g.vertexCount(), false); // sptSet[i] will be true if vertex i is included in shortest
                                                          // path tree or shortest distance from src to i is finalized

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
                double cost = g.rEdgeAttr(uidx, vp, 0);
                if (dist[uidx] + cost < dist[vp])
                {
                    dist[vp] = dist[uidx] + cost;
                    pred[vp] = uidx;
                }
            }
        }

        // std::pair<std::vector<int>,std:vector<double>> ret { std::makepred,dist}
        return std::make_pair(pred, dist);
    }

    std::pair<std::vector<int>, double>
    path(
        const cGraphData &g,
        const std::string &startName,
        const std::string &endName)
    {
        return path(
            g,
            g.find(startName),
            g.find(endName));
    }

    std::pair<std::vector<int>, double>
    path(
        const cGraphData &g,
        int start,
        int end)
    {
        std::vector<int> vpath;

        if (0 > start || start > g.vertexCount() ||
            0 > end || end > g.vertexCount())
            return std::make_pair(vpath, -1);

        // run the Dijsktra algorithm
        auto result = dijsktra(g, start);

        // check that end is reachable from start
        if (result.first[end] == -1)
            return std::make_pair(vpath, -1);

        vpath.push_back(end);
        int next = end;
        while (1)
        {
            next = result.first[next];
            vpath.push_back(next);
            if (next == start)
                break;
        }
        std::reverse(vpath.begin(), vpath.end());

        return std::make_pair(vpath, result.second[end]);
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
        spanTree.findorAdd(
            g.userName(v),
            g.userName(w),
            "1");
        // std::cout << "add span " << g.userName(v) << " " << g.userName(w) << "\n";
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

                    double cost = g.rEdgeAttr(v, w, 0);
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
            // std::cout << "add span " << g.userName(bestLink.first) << " " << g.userName(bestLink.second) << "\n";
            // std::cout << spanTree.text();
            visited[bestLink.first] = true;
            visited[bestLink.second] = true;
        }

        // std::cout << "spanTree return\n"
        //           << spanTree.text();
        return spanTree;
    }

    void dfs(
        const cGraphData &g,
        const std::string &startName,
        std::function<bool(int v)> visitor)
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

        int startIndex = g.find(startName);
        if (startIndex < 0)
            throw std::runtime_error(
                "dfs bad start " + startName);
        wait.push(startIndex);

        while (!wait.empty())
        {
            int v = wait.top();
            if (v < 0)
                throw std::runtime_error(
                    "dfs bad index 1");
            wait.pop();
            if (visited[v])
                continue;
            if (!visitor(v))
                break;
            visited[v] = true;

            for (int w : g.adjacentOut(v))
            {
                if (w < 0)
                    throw std::runtime_error(
                        "dfs bad index 2");
                if (!visited[w])
                    wait.push(w);
            }
        }
    }

    std::vector<std::vector<int>>
    dfs_cycle_finder(
        const cGraphData &g,
        const std::string &start)
    {
        if (!g.isDirected())
            throw std::runtime_error(
                "cGraph::dfs_cycle_finder invoked on undirected graph");

        std::vector<std::vector<int>> ret;

        // track visited vertices
        std::vector<bool> visited(g.vertexCount(), false);

        // vertices waiting to be processed
        std::stack<int> wait;

        // start at the beginning
        wait.push(g.find(start));

        // continue until no more vertices need processing
        while (!wait.empty())
        {
            int v = wait.top();
            wait.pop();
            if (!visited[v])
            {
                visited[v] = true;

                for (int w : g.adjacentOut(v))
                {
                    if (!visited[w])
                    {
                        wait.push(w);
                    }
                    else
                    {
                        // previously visited node, check for ancestor
                        auto cycle = path(g, w, v);
                        if (cycle.first.size() > 0)
                        {
                            // found a cycle
                            cycle.first.push_back(w);
                            ret.push_back(cycle.first);
                        }
                    }
                }
            }
        }
        return ret;
    }

    void cliques(
        const cGraphData &g,
        std::string &results)
    {
        // working copy on input graph
        auto work = g;

        // store for maximal clique collection
        std::vector<std::vector<int>> vclique;

        // true when all vertices have been moved into a clique
        bool finished = false;

        while (!finished)
        {
            std::vector<int> clique;

            while (!finished)
            {
                // std::cout << "work.nodeCount " << work.nodeCount() << " " << clique.size() << "\n";
                if (!clique.size())
                {
                    // start by moving an arbitrary node to the clique from the work graph
                    for (int vi = 0; vi < work.vertexCount(); vi++)
                    {
                        if (work.rVertexAttrString(vi, 0) == "deleted")
                            continue;
                        clique.push_back(vi);
                        work.wVertexAttr(vi, {"deleted"});
                        break;
                    }
                    continue;
                }
                bool found = false;

                // loop over nodes remaining in work graph
                finished = true;
                for (int u = 0; u < work.vertexCount(); u++)
                {
                    if (work.rVertexAttrString(u, 0) == "deleted")
                        continue;
                    finished = false;

                    // loop over nodes in clique
                    for (int v : clique)
                    {
                        if (work.find(u, v) >= 0 ||
                            work.find(v, u) >= 0)
                        {
                            // found node in work that is connected to clique nodes.
                            // move it to clique
                            std::cout << "add " << work.userName(u) << "\n";
                            clique.push_back(u);
                            work.wVertexAttr(u, {"deleted"});
                            found = true;
                            break;
                        }
                    }
                    if (found)
                        break; // found a node to add to clique
                }
                if (!found)
                    break; // no more nodes can be added, the clique is maximal
            }

            if (!clique.size())
                break; // did not find a clique

            // add to collection of maximal cliques
            vclique.push_back(clique);
        }

        // Display results
        std::stringstream ss;
        for (auto &c : vclique)
        {
            ss << "clique: ";
            for (int n : c)
                ss << g.userName(n) << " ";
            ss << "\n";
        }
        results = ss.str();
    }
    double flows(
        const cGraphData &g,
        const std::string &start,
        const std::string &end)
    {

        int totalFlow = 0;

        auto work = g;

        while (1)
        {
            // find path
            // std::cout << "links:\n" << linksText() << "\n";
            auto p = path(work, start, end);
            // std::cout << "pathsize " << myPath.size() << " ";
            if (!p.first.size())
                break;
            // std::cout << "Path " << pathText() << "\n";

            // maximum flow through path
            int maxflow = INT_MAX;
            int u = -1;
            int v;
            for (int v : p.first)
            {
                if (u >= 0)
                {
                    double cap = work.rEdgeAttr(u, v, 0);
                    if (cap < maxflow)
                    {
                        maxflow = cap;
                    }
                }
                u = v;
            }

            // consume capacity of links in path
            u = -1;
            for (int v : p.first)
            {
                if (u >= 0)
                {
                    double cap = work.rEdgeAttr(u, v, 0) - maxflow;
                    if (cap <= 0)
                    {
                        work.remove(u, v);
                    }
                    else
                    {
                        work.wEdgeAttr(
                            work.find(u, v),
                            {std::to_string(cap)});

                        if (!work.isDirected())
                            work.wEdgeAttr(
                                work.find(v, u),
                                {std::to_string(cap)});
                    }
                }
                u = v;
            }

            totalFlow += maxflow;
        }

        return totalFlow;
    }

}
