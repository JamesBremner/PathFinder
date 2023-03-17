#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <set>
#include "GraphTheory.h"

namespace raven
{
    namespace graph
    {

        void dijsktra(
            const cGraph &g,
            int start,
            std::vector<double> &dist,
            std::vector<int> &pred)
        {
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
                    double cost = atof(g.rEdgeAttr(g.find(uidx, vp), 0).c_str());
                    if (dist[uidx] + cost < dist[vp])
                    {
                        dist[vp] = dist[uidx] + cost;
                        pred[vp] = uidx;
                    }
                }
            }
        }

        std::pair<std::vector<int>, double>
        path(
            const cGraph &g,
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
            const cGraph &g,
            int start,
            int end)
        {
            std::vector<int> vpath;

            if (0 > start || start > g.vertexCount() ||
                0 > end || end > g.vertexCount())
                return std::make_pair(vpath, -1);

            // run the Dijsktra algorithm
            std::vector<double> dist;
            std::vector<int> pred;
            dijsktra(g, start, dist, pred);

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

        std::vector<std::vector<int>>
        allPaths(
            const cGraph &g,
            int start,
            int end)
        {
            std::vector<std::vector<int>> ret;

            // copy input graph to working graph
            cGraph work = g;

            bool fnew = true;
            while (fnew)
            {
                // find new path
                auto p = path(work, start, end).first;
                if (!p.size())
                    break;

                // check path is really new
                for (auto &prevPath : ret)
                {
                    if (std::equal(
                            prevPath.begin(), prevPath.end(),
                            p.begin()))
                    {
                        fnew = false;
                        break;
                    }
                }
                // check new path was found
                if (!fnew)
                    break;

                // add new path to return
                ret.push_back(p);

                // increment cost of path links
                for (int k = 1; k < p.size(); k++)
                {
                    int ei = work.find(p[k - 1], p[k]);
                    int cost = atoi(work.rEdgeAttr(ei, 0).c_str()) + 1;
                    work.wEdgeAttr(
                        ei,
                        {std::to_string(cost)});
                }
            }
            return ret;
        }

        void cSpanningTree::add(
            const cGraph &g,
            int v, int w)
        {
            mySpanningTree.findorAdd(v, w);
            mySpanningTree.wVertexName(v, g.userName(v));
            mySpanningTree.wVertexName(w, g.userName(w));
            myVertexSet.insert(v);
            myVertexSet.insert(w);
        }

        cGraph
        spanningTree(
            const cGraph &g,
            const std::string &startName)
        {
            cSpanningTree ST;

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
            ST.add(g, v, w);

            visited[v] = true;
            visited[w] = true;

            // while nodes remain outside of span
            while (g.vertexCount() > ST.vertexCount())
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
                    for (auto w : g.adjacentOut(v))
                    {
                        if (visited[w])
                            continue;
                        if (v > w)
                            continue;

                        // check edge exists
                        int ei = g.find(v, w);
                        if (ei < 0)
                            continue;

                        // track cheapest edge
                        double cost = atof(g.rEdgeAttr(ei, 0).c_str());
                        if (cost < min_cost)
                        {
                            min_cost = cost;
                            bestLink = std::make_pair(v, w);
                        }
                    }
                }

                // add cheapest link between node in tree to node not yet in tree
                ST.add(g, bestLink.first, bestLink.second);

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
                2 Take the top vertex of the stack and add it to the visited list.
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

        std::vector<std::vector<int>>
        dfs_cycle_finder(
            const cGraph &g)
        {
            if (!g.isDirected())
                throw std::runtime_error(
                    "cGraph::dfs_cycle_finder invoked on undirected graph");

            std::vector<std::vector<int>> ret;

            // track visited vertices
            std::vector<bool> visited(g.vertexCount(), false);

            while (true)
            {
                // check for unvisited vertices
                int startIndex = -1;
                for (int k = 0; k < visited.size(); k++)
                {
                    if (!visited[k])
                    {
                        startIndex = k;
                        break;
                    }
                }
                if (startIndex == -1)
                    break;

                // vertices waiting to be processed
                std::stack<int> wait;

                // start from an unvisited vertex
                wait.push(startIndex);

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
                                    cycle.first.push_back(w);

                                    // check this is a new cycle
                                    bool fnew = true;
                                    for (int kcycle = 0; kcycle < ret.size(); kcycle++)
                                    {
                                        if (ret[kcycle].size() != cycle.first.size())
                                            continue;
                                        bool fsame = true;
                                        for (int i = 0; i < cycle.first.size(); i++)
                                        {
                                            if (ret[kcycle][i] != cycle.first[i])
                                            {
                                                fsame = false;
                                                break;
                                            }
                                        }
                                        if (fsame)
                                        {
                                            fnew = false;
                                            break;
                                        }
                                    }
                                    if (fnew)
                                    {
                                        ret.push_back(cycle.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return ret;
        }

        void cliques(
            const cGraph &g,
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

        double
        flows(
            const cGraph &g,
            int start,
            int end,
            std::vector<int>& vEdgeFlow )
        {
            if (!g.isDirected())
                throw std::runtime_error(
                    "Flow calculation needs directed graph ( 2nd input line must be 'g')");

            int totalFlow = 0;

            // copy graph to working graph
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
                        double cap = atof(work.rEdgeAttr(work.find(u, v), 0).c_str());
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
                        double cap = atof(work.rEdgeAttr(work.find(u, v), 0).c_str()) - maxflow;
                        if (cap <= 0)
                        {
                            // link capacity filled, remove
                            work.remove(work.find(u, v));
                        }
                        else
                        {
                            work.wEdgeAttr(
                                u, v,
                                {std::to_string(cap)});
                        }
                    }
                    u = v;
                }

                totalFlow += maxflow;
            }

            vEdgeFlow.clear();
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                double f;
                if (work.dest(ei) == -1)
                    f = atof(g.rEdgeAttr(ei, 0).c_str());
                else
                {
                    double oc = atof(g.rEdgeAttr(ei, 0).c_str());
                    double wc = atof(work.rEdgeAttr(ei, 0).c_str());
                    f = oc - wc;
                }
                vEdgeFlow.push_back( f );
                // if (f > 0)
                //     std::cout << g.userName(g.source(ei))
                //               << " " << g.userName(g.dest(ei))
                //               << " " << f << "\n";
            }

            return totalFlow;
        }

        double multiflows(
            const cGraph &g,
            const std::vector<int> &vsource,
            int end)
        {
            double totalmultiflow = 0;
            std::vector<int> vEdgeFlow;
            for (int s : vsource)
            {
                totalmultiflow += flows(
                    g,
                    s,
                    end,
                    vEdgeFlow);
            }
            return totalmultiflow;
        }

        std::vector<std::vector<int>> sourceToSink(
            const cGraph &g)
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
                dijsktra(g, vi, dist, pred);

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

        double probs(cGraph &g, int end)
        {
            if (!g.isDirected())
                throw std::runtime_error(
                    "Probability calculation needs directed graph ( 2nd input line must be 'g')");

            // Mark all node probabilities as 'not yet calculated'
            std::string nyc("-1");
            for (int vi = 0; vi < g.vertexCount(); vi++)
                g.wVertexAttr(vi, {nyc});

            // loop over nodes
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                if (vi == end)
                    continue;

                // check for possible starting node
                // i.e one with out edges and no in edges
                if ((!g.adjacentOut(vi).size()) && (!g.adjacentIn(vi).size()))
                    continue;

                // iterate over all paths from starting node to target node
                for (auto &path : allPaths(
                         g,
                         vi,
                         end))
                {
                    // loop over nodes in path
                    for (int n : path)
                    {
                        if (n < 0)
                            continue;

                        // loop over inlinks
                        std::vector<double> vprob;
                        bool fOK = true;
                        for (int m : g.adjacentIn(n))
                        {
                            auto prevNodeProb = g.rVertexAttr(m, 0);
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
                                atof(g.rEdgeAttr(g.find(m, n), 0).c_str()));
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
                                g.userName(n) + " has more than 2 inlinks, please refactor input");
                        }

                        // save node probability
                        g.wVertexAttr(n, {std::to_string(nodeprob)});
                    }
                }
            }

            return atof(g.rVertexAttr(end, 0).c_str());
        }
        std::vector<std::string> alloc(cGraph &g)
        {
            // identify unique agents and tasks
            std::set<int> setAgent, setTask;
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                setAgent.insert(g.source(ei));
                setTask.insert(g.dest(ei));
            }

            // add link from start to each agent
            int start = g.add("start_alloc");
            for (int agent : setAgent)
                g.addEdge(start, agent);

            // add link from each task to end
            int end = g.add("end_alloc");
            for (int task : setTask)
                g.addEdge(task, end);

            // set capacity of every link to 1
            for (int ei = 0; ei < g.edgeCount(); ei++)
                g.wEdgeAttr(ei, {"1"});

            // assign agents to tasks by calculating the maximum flow
            std::vector<int> vEdgeFlow;
            flows(g, start, end, vEdgeFlow);

            std::vector<std::string> ret;
            for (int ei = 0; ei < g.edgeCount(); ei++)
            {
                if( vEdgeFlow[ei] <= 0 )
                    continue;

                int s = g.source(ei);
                int d = g.dest(ei);
                if (s == start)
                    continue;
                if (d == end)
                    continue;
                ret.push_back(g.userName(s));
                ret.push_back(g.userName(d));

            }
            return ret;
        }
    }
}
