#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <set>
#include <queue>
#include "GraphTheory.h"

namespace raven
{
    namespace graph
    {
        void dijsktra(
            const cGraph &g,
            const std::vector<double> &edgeWeight,
            int start,
            std::vector<double> &dist,
            std::vector<int> &pred)
        {
            if (edgeWeight.size() < g.edgeCount())
                throw std::runtime_error(
                    "dijsktra bad edge weights");

            // shortest distance from start to each node
            dist.clear();
            dist.resize(g.vertexCount(), INT_MAX);

            // previous node on shortest path to each node
            pred.clear();
            pred.resize(g.vertexCount(), -1);

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
                    double cost = edgeWeight[g.find(uidx, vp)];
                    if (dist[uidx] + cost < dist[vp])
                    {
                        dist[vp] = dist[uidx] + cost;
                        pred[vp] = uidx;
                    }
                }
            }
        }

        std::pair<std::vector<int>, double>
        path(sGraphData &gd)
        {
            if (gd.startName.empty() || gd.endName.empty())
                throw std::runtime_error(
                    "path: no start or end");

            int start = gd.g.find(gd.startName);
            int end = gd.g.find(gd.endName);
            if (start < 0 || end < 0)
                throw std::runtime_error(
                    "path: bad start or end");

            std::vector<int> vpath;

            if (0 > start || start > gd.g.vertexCount() ||
                0 > end || end > gd.g.vertexCount())
                return std::make_pair(vpath, -1);
            if (start == end)
            {
                vpath.push_back(start);
                return std::make_pair(vpath, 0);
            }

            // run the Dijsktra algorithm
            std::vector<double> dist;
            std::vector<int> pred;
            dijsktra(gd.g, gd.edgeWeight, start, dist, pred);

            // check that end is reachable from start
            if (pred[end] == -1)
                return std::make_pair(vpath, -1);

            vpath.push_back(end);
            int next = end;
            while (1)
            {
                next = pred[next];
                vpath.push_back(next);
                if (next == start)
                    break;
            }
            std::reverse(vpath.begin(), vpath.end());

            return std::make_pair(vpath, dist[end]);
        }

        static void combine_yen(
            path_cost_t &spur,
            const path_cost_t &prev,
            int rootlength,
            const sGraphData &gd)
        {
            spur.first.insert(
                spur.first.begin(),
                prev.first.begin(), prev.first.begin() + rootlength - 1);
            spur.second = 0;
            for (int v = 1; v < spur.first.size(); v++)
                spur.second += gd.edgeWeight[gd.g.find(
                    spur.first[v - 1], spur.first[v])];
        }
        static bool isNewPath_yen(
            const path_cost_t &test,
            const vPath_t &vShortestPaths)
        {
            return (
                std::find_if(
                    vShortestPaths.begin(), vShortestPaths.end(),
                    [&](const path_cost_t &p)
                    {
                        return std::equal(
                            p.first.begin(), p.first.end(),
                            test.first.begin());
                    }) == vShortestPaths.end());
        }

        vPath_t
        allPaths(sGraphData &gd)
        {
            vPath_t vShortestPaths;
            vPath_t vPotentialPaths;

            // Dijsktra gives the very shortest path
            vShortestPaths.push_back(path(gd));

            // loop looking for next shortest path
            while (true)
            {
                // initialize
                vPotentialPaths.clear();
                auto gdwork = gd;

                // loop over previously found shortest paths
                for (auto &foundPath : vShortestPaths)
                {
                    // loop over the previously found path
                    for (int rootlength = 1; rootlength < foundPath.first.size(); rootlength++)
                    {
                        // remove link from spur node used by previous path
                        gdwork.g.remove(
                            foundPath.first[rootlength - 1],
                            foundPath.first[rootlength]);

                        // find shortest path to destination ( spur path )
                        // without using the link
                        // dijsktra
                        gdwork.startName = gd.g.userName(foundPath.first[rootlength - 1]);
                        path_cost_t spurPath = path(gdwork);

                        // check spur path found
                        if (!spurPath.first.size())
                            continue;

                        // combine root and spur paths
                        combine_yen(
                            spurPath,
                            foundPath,
                            rootlength,
                            gd);

                        // check this is new path
                        if (isNewPath_yen(spurPath, vShortestPaths))
                            vPotentialPaths.push_back(spurPath);
                    }
                }

                // no more paths check
                if (!vPotentialPaths.size())
                    break;

                // Add shortest potential path to output
                vShortestPaths.push_back(
                    *std::min_element(
                        vPotentialPaths.begin(), vPotentialPaths.end(),
                        [](path_cost_t &a, path_cost_t &b)
                        {
                            return a.second < b.second;
                        }));
            }
            return vShortestPaths;
        }

        void cSpanningTree::add(
            const cGraph &g,
            int v, int w)
        {
            mySpanningTree.add(g.userName(v), g.userName(w));
            myVertexSet.insert(v);
            myVertexSet.insert(w);
        }

        cGraph
        spanningTree(sGraphData &gd)
        {
            // std::cout << "spanning Tree " << startName << "\n";

            if (gd.edgeWeight.size() < gd.g.edgeCount())
                throw std::runtime_error(
                    "spanningTree bad edge weights");

            // copy vertices from input graph to spanning tree
            cSpanningTree ST;
            for (int kv = 0; kv < gd.g.vertexCount(); kv++)
                ST.mySpanningTree.add(gd.g.userName(kv));

            int start = gd.g.find(gd.startName);

            // track visited vertices
            std::vector<bool> visited(gd.g.vertexCount(), false);

            // add start vertex to spanning tree
            int v = start;
            auto va = gd.g.adjacentOut(v);
            if (!va.size())
                throw std::runtime_error(
                    "spanning tree start vertex unconnected");
            visited[v] = true;

            // while nodes remain outside of span
            while (gd.g.vertexCount() > ST.vertexCount())
            {
                double min_cost = INT_MAX;
                std::pair<int, int> bestLink;
                bestLink.first = -1;

                // loop over nodes in span
                for (int v = 0; v < gd.g.vertexCount(); v++)
                {
                    if (!visited[v])
                        continue;

                    // loop over adjacent nodes not in span
                    for (auto w : gd.g.adjacentOut(v))
                    {
                        // std::cout << "try " << g.userName(v) <<" "<< g.userName(w) << "\n";
                        if (visited[w])
                            continue;

                        // check edge exists
                        int ei = gd.g.find(v, w);
                        if (ei < 0)
                            continue;

                        // track cheapest edge
                        double cost = gd.edgeWeight[ei];
                        if (cost < min_cost)
                        {
                            min_cost = cost;
                            bestLink = std::make_pair(v, w);
                        }
                    }
                }

                if (bestLink.first == -1)
                {
                    std::cout << "spanning tree starting from " << gd.startName << " cannot reach ";
                    for (int v = 0; v < visited.size(); v++)
                        if (!visited[v])
                            std::cout << gd.g.userName(v) << " ";
                    std::cout << "\n";
                    // std::cout << g.text() << "\nPartial ST\n";
                    // std::cout << ST.mySpanningTree.text();
                    ST.mySpanningTree.clear();
                    return ST.mySpanningTree;
                }

                // add cheapest link between node in tree to node not yet in tree
                ST.add(gd.g, bestLink.first, bestLink.second);

                visited[bestLink.first] = true;
                visited[bestLink.second] = true;
            }

            return ST.mySpanningTree;
        }

        void dfs(
            const cGraph &g,
            int startIndex,
            std::function<bool(int v)> visitor)
        {
            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            // vertices waiting to be visited
            std::stack<int> wait;

            /*  1 Start by putting one of the graph's vertices on top of a stack.
                2 Take the top vertex off the stack and add it to the visited list.
                3 Add adjacent vertices which aren't in the visited list to the top of the stack.
                4 Keep repeating steps 2 and 3 until the stack is empty.
            */

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

        void bfs(
            const cGraph &g,
            int startIndex,
            std::function<bool(int v)> visitor)
        {
            // queue of visited vertices with unsearched children
            std::queue<int> Q;

            // track nodes that have been visited
            // prevent getting caught going around and around a cycle
            std::vector<bool> visited(g.vertexCount(), false);

            // start at start
            Q.push(startIndex);
            visited[startIndex] = true;
            if (!visitor(startIndex))
                return;

            // loop while the queue is not empty
            while (!Q.empty())
            {
                // get current vertex from front of queue
                int v = Q.front();
                Q.pop();

                // loop over vertices reachable from current vertex
                for (int u : g.adjacentOut(v))
                {
                    if (!visited[u])
                    {
                        // add to queue and mark visited
                        Q.push(u);
                        visited[u] = true;

                        if (!visitor(v))
                            return;
                    }
                }
            }
        }

        std::vector<std::vector<int>>
        dfs_cycle_finder(sGraphData &gd)
        {
            // store for found cycles, vertex indices in order reached.
            std::vector<std::vector<int>> ret;

            // store found cycle signatures
            std::vector<std::vector<int>> vfoundCycleSignature;

            // track visited vertices
            std::vector<bool> visited(gd.g.vertexCount(), false);

            /* cycles exist whatever the edge weights
            So construct default edge weights for all edges
            allowing for all possible edges
            */
            gd.edgeWeight.clear();
            gd.edgeWeight.resize(
                2 * gd.g.vertexCount() * gd.g.vertexCount(),
                1);

            // working graph data
            // used to find cycle when previously visited vertex encountered
            sGraphData work;
            work.g = gd.g;
            work.edgeWeight = gd.edgeWeight;

            /* loop until all vertices have been visited

            This is required for graphs that are not fully connected
            i.e. not every vertex can be reached from every other

            */
            while (true)
            {
                int startIndex;
                if (gd.startName.empty())
                {
                    // find unvisited vertex to start the DFS from
                    auto it = std::find(
                        visited.begin(),
                        visited.end(),
                        false);
                    if (it == visited.end())
                    {
                        // all vertices have been visited - done
                        break;
                    }
                    startIndex = it - visited.begin();
                }
                else
                {
                    startIndex = gd.g.find(gd.startName);
                }

                // vertices waiting to be processed
                std::stack<int> wait;

                // start from an unvisited vertex
                wait.push(startIndex);

                // continue until no more vertices can be reached from the starting vertex
                while (!wait.empty())
                {
                    // visit vertex at top of stack
                    int v = wait.top();
                    wait.pop();
                    visited[v] = true;

                    // loop over vertices reachable with one hop
                    for (int w : gd.g.adjacentOut(v))
                    {
                        if (!visited[w])
                        {
                            // push unvisited vertex onto stack to be visited later
                            wait.push(w);
                            continue;
                        }

                        /* previously visited node

                        before carrying on we need to if this is a novel cycle
                        apply Dijsktra algorithm to find shortest path
                        from w back to the common ancestor and then around to v again

                        */
                        std::vector<int> cycle;
                        if (!gd.g.isDirected())
                        {
                            // for undirected  graphs
                            // remove reverse edge
                            // so the path is forced to go the long way around back to start

                            work.startName = gd.g.userName(w);
                            work.endName = gd.g.userName(v);
                            work.g.remove(w, v);
                            cycle = path(work).first;

                            // restore removed edge
                            work.g = gd.g;
                        }
                        else
                        {
                            work.startName = gd.g.userName(w);
                            work.endName = gd.g.userName(v);
                            cycle = path(work).first;
                        }

                        // ignore "cycles" that just go back and forth over one edge
                        if (cycle.size() < 2)
                            continue;

                        // create cycle signature
                        // this is a list of the vertex indices in the cycle sorted in numerical order
                        // i.e. the signature is the same no matter where the start begins
                        std::vector<int> signature = cycle;
                        std::sort(signature.begin(), signature.end());

                        // check this is a new cycle
                        // loop over previously found cycles
                        bool fnew = true;
                        for (auto &foundSignature : vfoundCycleSignature)
                        {
                            // check cycle length
                            if (foundSignature.size() != signature.size())
                                continue;

                            // check cycle signature
                            if (std::equal(
                                    foundSignature.begin(),
                                    foundSignature.end(),
                                    signature.begin()))
                            {
                                // this cycle was found previously
                                fnew = false;
                                break;
                            }
                        }
                        if (!fnew)
                            continue;

                        // this is a novel cycle

                        // close the cycle
                        cycle.push_back(w);

                        // store in list of cycles found
                        ret.push_back(cycle);

                        // store the signature to prevent duplicates being stored
                        vfoundCycleSignature.push_back(signature);
                    }
                }
                if (!gd.startName.empty())
                {
                    std::vector<std::vector<int>> cycles_with_start;
                    for (auto &c : ret)
                    {
                        if (std::find(c.begin(), c.end(), startIndex) != c.end())
                            cycles_with_start.push_back(c);
                    }
                    ret = cycles_with_start;
                    break;
                }
            }
            return ret;
        }

        std::vector<int>
        bfsPath(sGraphData &gd)
        {
            // the path will be stored here
            std::vector<int> path;

            // queue of visited vertices with unsearched children
            std::queue<int> Q;

            // track nodes that have been visited
            // prevent getting caught going around and around a cycle
            std::vector<bool> visited(gd.g.vertexCount(), false);

            // store the vertex which every visited vertex was reached from
            std::vector<int> pred(gd.g.vertexCount(), -1);

            // get vertex indices from user names
            int start = gd.g.find(gd.startName);
            int dest = gd.g.find(gd.endName);

            // start at start
            int v = start;
            Q.push(v);
            visited[v] = true;

            // loop while the queue is not empty
            while (!Q.empty())
            {
                // get current vertex from front of queue
                v = Q.front();
                Q.pop();

                // loop over vertices reachable from current vertex
                for (int u : gd.g.adjacentOut(v))
                {
                    if (u == dest)
                    {
                        // reached the destination, no need to search further
                        pred[u] = v;
                        std::queue<int> empty;
                        std::swap(Q, empty);
                        break;
                    }
                    if (!visited[u])
                    {
                        // visit to a new node
                        // because this is BFS, the first visit will be from
                        // the previous node on the shortest path

                        // add to queue, record predeccor vertex, and mark visited
                        Q.push(u);
                        pred[u] = v;
                        visited[u] = true;
                    }
                }
            }

            if (pred[dest] == -1)
            {
                // destination not reachable
                return path;
            }

            // extract path by backtracking from destination to start
            v = dest;
            while (true)
            {
                path.push_back(v);
                if (v == start)
                    break;
                v = pred[v];
            }

            // flip path to start -> destination
            std::reverse(path.begin(), path.end());

            return path;
        }

        std::vector<path_t> components(
            const cGraph &g,
            bool fclique)
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
                            if (work.rVertexAttr(vi, 0) == "deleted")
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
                        if (work.rVertexAttr(u, 0) == "deleted")
                            continue;
                        finished = false;

                        if (!fclique)
                        {
                            // loop over nodes in clique
                            for (int v : clique)
                            {
                                if (work.find(u, v) >= 0 ||
                                    work.find(v, u) >= 0)
                                {
                                    // found node in work that is connected to clique nodes.
                                    // move it to clique
                                    //std::cout << "add " << work.userName(u) << " ";
                                    clique.push_back(u);
                                    work.wVertexAttr(u, {"deleted"});
                                    found = true;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // loop over all nodes in clique
                            bool adjacentFound = true;
                            for (int v : clique)
                            {
                                if (work.find(u, v) == -1 &&
                                    work.find(v, u) == -1)
                                {
                                    adjacentFound = false;
                                    break;
                                }
                            }
                            if (adjacentFound)
                            {
                                // found node in work that is directly connected to all clique nodes.
                                // move it to clique
                                //std::cout << "add " << work.userName(u) << "\n";
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

                std::cout << "component found\n";
            }

            return vclique;
        }

        double
        flows(
            sGraphData &gd,
            std::vector<int> &vEdgeFlow)
        {
            if (!gd.g.isDirected())
                throw std::runtime_error(
                    "Flow calculation needs directed graph ( 2nd input line must be 'g')");

            auto edgeCapacity = gd.edgeWeight; // the initial link capacities

            int totalFlow = 0;

            // copy graph to working graph
            auto work = gd;

            while (1)
            {
                /* find shortest path with available capacity

                This is the Edmonds–Karp implementation of the Ford–Fulkerson method
                It uses breadth first searching so the paths are found in a defined order
                rather than a 'random' order depending on how the links are stored in the graph data structure

                https://en.wikipedia.org/wiki/Edmonds%E2%80%93Karp_algorithm

                */

                auto p = bfsPath(work);

                if (!p.size())
                    break;

                // maximum flow through path
                int maxflow = INT_MAX;
                int u = -1;
                int v;
                for (int v : p)
                {
                    if (u >= 0)
                    {
                        double cap = edgeCapacity[work.g.find(u, v)];
                        if (cap < maxflow)
                        {
                            maxflow = cap;
                        }
                    }
                    u = v;
                }

                // consume capacity of links in path
                u = -1;
                for (int v : p)
                {
                    if (u >= 0)
                    {
                        int ei = work.g.find(u, v);
                        double cap = edgeCapacity[ei] - maxflow;
                        if (cap <= 0)
                        {
                            // link capacity filled, remove
                            work.g.remove(u, v);
                        }
                        else
                        {
                            edgeCapacity[ei] = cap;
                        }
                    }
                    u = v;
                }

                totalFlow += maxflow;
            }

            vEdgeFlow.clear();
            for (int ei = 0; ei < gd.g.edgeCount(); ei++)
            {
                double f;
                if (work.g.dest(ei) == -1)
                    f = gd.edgeWeight[ei];
                else
                {
                    double oc = gd.edgeWeight[ei];
                    double wc = edgeCapacity[ei];
                    f = oc - wc;
                }

                //         std::cout << g.userName(g.src(ei))
                //   << " " << g.userName(g.dest(ei))
                //   << " " << f << "\n";

                vEdgeFlow.push_back(f);
            }

            return totalFlow;
        }

        double multiflows(sGraphData &gd)
        {
            // total flow on each link from all sources
            std::vector<int> vMultiFlow(gd.g.edgeCount());

            // flow on each link from current source
            std::vector<int> vEdgeFlow(gd.g.edgeCount());

            // working copy of graph
            sGraphData work = gd;

            // loop over sources
            for (auto &s : gd.multiStart)
            {
                // reduce link capacities by flow from previous generator
                for (int k = 0; k < vEdgeFlow.size(); k++)
                    work.edgeWeight[k] -= vEdgeFlow[k];

                // calculate flow from this generator
                vEdgeFlow.clear();
                work.startName = s;
                flows(
                    work,
                    vEdgeFlow);

                // add to total flow
                for (int k = 0; k < vEdgeFlow.size(); k++)
                    vMultiFlow[k] += vEdgeFlow[k];
            }

            // display total flow through each link
            for (int k = 0; k < vMultiFlow.size(); k++)
            {
                std::cout << gd.g.userName(gd.g.src(k))
                          << " -> " << gd.g.userName(gd.g.dest(k))
                          << " " << vMultiFlow[k] << "\n";
            }
            return 0;
        }

        std::vector<std::vector<int>> sourceToSink(
            const cGraph &g,
            const std::vector<double> &edgeWeight)
        {
            std::vector<std::vector<int>> ret;

            // find sinks
            std::vector<int> vsink;
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                if (!g.adjacentOut(vi).size())
                    vsink.push_back(vi);
            }

            // loop over vertices
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                // check for source
                if (g.adjacentIn(vi).size())
                    continue;

                // find path to every other vertex
                std::vector<double> dist;
                std::vector<int> pred;
                dijsktra(g, edgeWeight, vi, dist, pred);

                // find connected sinks
                std::vector<int> vConnected;
                vConnected.push_back(vi);
                for (int si : vsink)
                {
                    if (pred[si] >= 0)
                        vConnected.push_back(si);
                }
                ret.push_back(vConnected);
            }
            return ret;
        }

        double probs(sGraphData &gd)
        {
            if (!gd.g.isDirected())
                throw std::runtime_error(
                    "Probability calculation needs directed graph ( 2nd input line must be 'g')");

            int end = gd.g.find(gd.endName);

            // Mark all node probabilities as 'not yet calculated'
            std::string nyc("-1");
            for (int vi = 0; vi < gd.g.vertexCount(); vi++)
                gd.g.wVertexAttr(vi, {nyc});

            // loop over nodes
            for (int vi = 0; vi < gd.g.vertexCount(); vi++)
            {
                if (vi == end)
                    continue;

                // check for possible starting node
                // i.e one with out edges and no in edges
                if ((!gd.g.adjacentOut(vi).size()) && (!gd.g.adjacentIn(vi).size()))
                    continue;

                // iterate over all paths from starting node to target node
                gd.startName = gd.g.userName(vi);
                for (auto &path : allPaths(gd))
                {
                    // loop over nodes in path
                    for (int n : path.first)
                    {
                        if (n < 0)
                            continue;

                        // loop over inlinks
                        std::vector<double> vprob;
                        bool fOK = true;
                        for (int m : gd.g.adjacentIn(n))
                        {
                            auto prevNodeProb = gd.g.rVertexAttr(m, 0);
                            if (prevNodeProb == "-1")
                            {
                                // the previous node probability has not been calculated yet
                                // no need to look at any more inlinks
                                fOK = false;
                                break;
                            }
                            // store the probability contribution from this inlink
                            // it is the product of the source node proabability and the link probability
                            vprob.push_back(
                                atof(prevNodeProb.c_str()) *
                                gd.edgeWeight[gd.g.find(m, n)]);
                        }
                        // check if there is enough information
                        // to calculate the probability for this node
                        if (!fOK)
                            break;

                        // all the previous nodes are calculated
                        // calculate this node's probability
                        double nodeprob = -1;
                        switch (vprob.size())
                        {
                        case 0:
                            // starting node, assume probability of 100%
                            nodeprob = 1;
                            break;

                        case 1:
                            // one inlink, prob is previous node prob times link probability
                            nodeprob = vprob[0];
                            break;

                        case 2:
                            // two inlinks
                            nodeprob =
                                vprob[0] + vprob[1] - vprob[0] * vprob[1];
                            break;

                        default:
                            /*  More then two inlinks
                            The code does not handle this
                            but note that multiple inlinks can alwayd be reduced to a series of
                            nodes with 2 inlinks
                            */
                            throw std::runtime_error(
                                gd.g.userName(n) + " has more than 2 inlinks, please refactor input");
                        }

                        // save node probability
                        gd.g.wVertexAttr(n, {std::to_string(nodeprob)});
                    }
                }
            }

            return atof(gd.g.rVertexAttr(end, 0).c_str());
        }
        cGraph alloc(
            sGraphData &gd)
        {
            // identify unique agents and tasks
            std::set<int> setAgent, setTask;
            for (int ei = 0; ei < gd.g.edgeCount(); ei++)
            {
                setAgent.insert(gd.g.src(ei));
                setTask.insert(gd.g.dest(ei));
            }

            // add link from start to each agent
            int start = gd.g.add("start_alloc");
            for (int agent : setAgent)
                gd.g.add(start, agent);

            // add link from each task to end
            int end = gd.g.add("end_alloc");
            for (int task : setTask)
                gd.g.add(task, end);

            gd.startName = "start_alloc";
            gd.endName = "end_alloc";

            // set capacity of every link to 1
            gd.edgeWeight.clear();
            gd.edgeWeight.resize(gd.g.edgeCount(), 1);

            // assign agents to tasks by calculating the maximum flow
            std::vector<int> vEdgeFlow;
            flows(gd, vEdgeFlow);

            // construct return graph with links from agents to their assigned tasks
            cGraph gret;
            gret.directed();
            for (int ei = 0; ei < gd.g.edgeCount(); ei++)
            {
                if (vEdgeFlow[ei] <= 0)
                    continue;

                int s = gd.g.src(ei);
                int d = gd.g.dest(ei);

                // skip links from/to source or sink
                if (s == start)
                    continue;
                if (d == end)
                    continue;

                // add assignment edge
                gret.add( 
                    gd.g.userName(s),
                    gd.g.userName(d));
            }
            return gret;
        }

        std::vector<int> euler(const cGraph &g)
        {
            // firewall
            if (!g.isDirected())
                throw std::runtime_error(
                    "euler:  needs directed graph ( 2nd input line must be 'g')");
            for (int vi = 0; vi < g.vertexCount(); vi++)
                if (g.adjacentIn(vi).size() != g.adjacentOut(vi).size())
                    throw std::runtime_error(
                        "euler: every vertex in-degree must equal out-degree");

            // working copy of graph
            cGraph work = g;

            // list to store circuit
            std::vector<int> circuit;

            // start at first vertex
            int curr_v = 0;

            while (1)
            {
                // add vertex to circuit
                circuit.push_back(curr_v);

                // find next vertex along unused edge
                auto vadj = work.adjacentOut(curr_v);
                if (!vadj.size())
                    break;
                int next_v = vadj[0];

                // remove used edge
                work.remove(curr_v, next_v);

                // continue from new vertex
                curr_v = next_v;
            }

            return circuit;
        }

        std::vector<int> vertexCover(const cGraph &g)
        {
            if (g.isDirected())
                throw std::runtime_error(
                    "vertexCover works only on undirected graphs");

            // output store
            std::vector<int> ret;
            std::set<int> vset;

            // working copy of input graph
            auto work = g;

            // The nodes that connect leaf nodes to the rest of the graph must be in cover set
            for (int leaf = 0; leaf < g.vertexCount(); leaf++)
            {
                // check for leaf vertex
                auto ns = g.adjacentOut(leaf);
                if (ns.size() != 1)
                    continue;

                // add to cover set
                vset.insert(ns[0]);
                // std::cout << "added " << work.userName(ns[0]) << "\n";
            }

            // loop over links
            for (auto l : work.edgeList())
            {
                if (vset.find(l.first) != vset.end() ||
                    vset.find(l.second) != vset.end())
                    continue;

                // add node with greatest degree to cover set
                auto sun = work.adjacentOut(l.first);
                auto svn = work.adjacentOut(l.second);
                int v = l.first;
                if (svn.size() > sun.size())
                    v = l.second;
                // std::cout << "added " << work.userName(v) << "\n";
                vset.insert(v);
            }

            for (int v : vset)
                ret.push_back(v);
            return ret;
        }

        cTSP::cTSP(raven::graph::cGraph &inputGraph,
                   const std::vector<double> &vEdgeWeight)
            : g(inputGraph),
              myEdgeWeight(vEdgeWeight),
              final_res(INT_MAX)
        {
        }

        // This function sets up final_path[]
        std::vector<int> cTSP::calculate()
        {

            // Calculate initial lower bound for the root node
            // using the formula 1/2 * (sum of first min +
            // second min) for all edges.
            // Also initialize the curr_path and visited array
            int curr_bound = 0;

            curr_path.clear();
            curr_path.resize(g.vertexCount() + 1, -1);
            visited.clear();
            visited.resize(g.vertexCount(), false);

            // Compute initial bound
            // auto g = makeGraph( adj );
            for (int i = 0; i < g.vertexCount(); i++)
                curr_bound += (firstMin(i) +
                               secondMin(i));

            // Rounding off the lower bound to an integer
            curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;

            // We start at vertex 1 so the first vertex
            // in curr_path[] is 0
            visited[0] = true;
            curr_path[0] = 0;

            // Call to TSPRec for curr_weight equal to
            // 0 and level 1
            TSPRec(curr_bound, 0, 1);

            return final_path;
        }
        // Function to find the minimum edge cost
        // having an end at the vertex i
        int cTSP::firstMin(int i)
        {
            int min = INT_MAX;
            for (int k : g.adjacentOut(i))
            {
                if (k != i)
                {
                    int c = edgeWeight(i, k);
                    if (c < min)
                        min = c;
                }
            }
            return min;
        }

        // function to find the second minimum edge cost
        // having an end at the vertex i
        int cTSP::secondMin(int i)
        {
            int first = INT_MAX, second = INT_MAX;
            for (int j = 0; j < g.vertexCount(); j++)
            {
                if (i == j)
                    continue;

                int c = edgeWeight(i, j);
                if (c <= first)
                {
                    second = first;
                    first = c;
                }
                else if (c <= second &&
                         c != first)
                    second = c;
            }
            return second;
        }

        // recursive function that takes as arguments:
        // curr_bound -> lower bound of the root node
        // curr_weight-> stores the weight of the path so far
        // level-> current level while moving in the search
        //		 space tree

        void cTSP::TSPRec(int curr_bound, int curr_weight,
                          int level)
        {
            // base case is when we have reached level N which
            // means we have covered all the nodes once
            if (level == g.vertexCount())
            {
                // check if there is an edge from last vertex in
                // path back to the first vertex
                if (g.find(curr_path[level - 1], curr_path[0]) >= 0)
                {
                    // curr_res has the total weight of the
                    // solution we got
                    int curr_res = curr_weight +
                                   edgeWeight(curr_path[level - 1], curr_path[0]);

                    // Update final result and final path if
                    // current result is better.
                    if (curr_res < final_res)
                    {
                        final_path.clear();
                        for (int v : curr_path)
                            if (v >= 0)
                                final_path.push_back(v);
                        final_path.push_back(curr_path[0]);
                        final_res = curr_res;
                    }
                }
                return;
            }

            // for any other level iterate for all vertices to
            // build the search space tree recursively
            for (int i = 0; i < g.vertexCount(); i++)
            {
                // Consider next vertex if it is not same (diagonal
                // entry in adjacency matrix and not visited
                // already)
                if (g.find(curr_path[level - 1], i) >= 0 &&
                    visited[i] == false)
                {
                    int temp = curr_bound;
                    curr_weight += edgeWeight(curr_path[level - 1], i);

                    // different computation of curr_bound for
                    // level 2 from the other levels
                    if (level == 1)
                        curr_bound -= ((firstMin(curr_path[level - 1]) +
                                        firstMin(i)) /
                                       2);
                    else
                        curr_bound -= ((secondMin(curr_path[level - 1]) +
                                        firstMin(i)) /
                                       2);

                    // curr_bound + curr_weight is the actual lower bound
                    // for the node that we have arrived on
                    // If current lower bound < final_res, we need to explore
                    // the node further
                    if (curr_bound + curr_weight < final_res)
                    {
                        curr_path[level] = i;
                        visited[i] = true;

                        // call TSPRec for the next level
                        TSPRec(curr_bound, curr_weight, level + 1);
                    }

                    // Else we have to prune the node by resetting
                    // all changes to curr_weight and curr_bound
                    curr_weight -= edgeWeight(curr_path[level - 1], i);
                    curr_bound = temp;

                    // Also reset the visited array
                    visited.clear();
                    visited.resize(g.vertexCount(), false);

                    for (int j = 0; j <= level - 1; j++)
                        visited[curr_path[j]] = true;
                }
            }
        }

        int cTSP::edgeWeight(int i, int j) const
        {
            return myEdgeWeight[g.find(i, j)];
        }

    }
}
