#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "cGraph.h"
#include "GraphTheory.h"
#include "cRunWatch.h"

raven::graph::cGraph theGraph;
std::vector<double> theEdgeWeight;

std::vector<std::string> tokenize(const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

std::vector<std::string> userQuery()
{
    std::cout << "type query> ";
    std::string line;
    getline(std::cin, line);
    return tokenize(line);
}

void displayStatus()
{
    std::cout << "\n"
              << theGraph.vertexCount() << " vertices "
              << theGraph.edgeCount() << " edges "
              << "\n\n";
}

void gen(const std::vector<std::string> &q)
{
    int vmax = atoi(q[1].c_str());
    theGraph.clear();
    for (int k = 0; k < vmax; k++)
    {
        theGraph.add("V" + std::to_string(k));
    }
    for (int k = 0; k < vmax; k++)
    {
        for (int kl = 0; kl < 3; kl++)
        {
            theGraph.add(rand() % vmax, rand() % vmax);
        }
    }
    displayStatus();
}

void genEuler(const std::vector<std::string> &q)
{
    int vmax = atoi(q[1].c_str());
    theGraph.clear();
    theGraph.directed();
    for (int k = 0; k < vmax; k++)
    {
        theGraph.add("V" + std::to_string(k));
    }
    int v1 = 0;
    for (int k = 0; k < vmax; k++)
    {
        int v2 = v1;
        while (v2 == v1)
            v2 = rand() % vmax;
        theGraph.add(v1, v2);
        v1 = v2;
    }
    theGraph.add(v1, 0);

    displayStatus();
}

void add(const std::vector<std::string> &q)
{
    if (q.size() < 3)
        return;
    if (q[1] == "random")
    {
        // add specified number of random links

        // create a script to add the requested links
        std::vector<std::pair<int, int>> vedge;
        int max = theGraph.vertexCount();
        srand(time(NULL));
        for (int k = 0; k < atoi(q[2].c_str()); k++)
        {
            vedge.push_back(std::make_pair(
                rand() % max, rand() % max));
            // std::cout << " add " << vedge.back().first <<" " << vedge.back().second << "\n";
        }

        // run the script
        {
            for (auto &p : vedge)
            {
                raven::set::cRunWatch aWatcher("randomAdd");
                theGraph.add(p.first, p.second);
            }
        }
    }
    else
    {
        // add one specified link

        theGraph.add(q[1], q[2]);
    }

    displayStatus();
}

void display(const std::vector<std::string> &q)
{
    std::cout << "vertex value average\n";
    for (int v = 0; v < theGraph.vertexCount(); v++)
    {
        std::cout << theGraph.userName(v)
                  << "\t" << theGraph.rVertexAttr(v, 0)
                  << "\t" << theGraph.rVertexAttr(v, 1) << "\n";
    }
    std::cout << "\n";

    // std::cout << theGraph.text();
}

void read(const std::vector<std::string> &q)
{
    if (q.size() < 2)
        return;
    theGraph.directed();
    std::ifstream ifs(q[1]);
    if (!ifs.is_open())
    {
        std::cout << "Cannot open " + q[1] << "\n";
        return;
    }
    std::string line;
    int count = 0;
    while (getline(ifs, line))
    {
        auto tokens = tokenize(line);
        if (tokens.size() < 3)
            continue;
        if (tokens[0].find("#") != -1)
            continue;

        // int v1 = theGraph.add(tokens[1]);
        // theGraph.wVertexAttr(v1, {std::to_string(rand() % 10), "0", "0"});
        // int v2 = theGraph.add(tokens[2]);
        // theGraph.wVertexAttr(v2, {std::to_string(rand() % 10), "0", "0"});
        // theGraph.add(v1, v2);

        // int v1 = theGraph.add(tokens[1]);
        // int v2 = theGraph.add(tokens[2]);
        int v1 = atoi(tokens[1].c_str());
        int v2 = atoi(tokens[2].c_str());

        theGraph.add(v1, v2);

        count++;
        if (count % 10000 == 0)
            std::cout << count << " links read\n";
    }
    displayStatus();
}

void cycles()
{
    raven::graph::sGraphData gd;
    gd.g = theGraph;
    auto cycles = dfs_cycle_finder(gd);
    std::cout << cycles.size() << " cycles\n";
}

void s2s()
{
    auto s2s = sourceToSink(theGraph, theEdgeWeight);
    std::cout << s2s.size() << " sources\n";
}

void span()
{
    raven::graph::sGraphData gd;
    gd.g = theGraph;
    gd.startName = theGraph.userName(0);
    raven::graph::spanningTree(gd);
}

void paths()
{
    std::vector<double> dist;
    std::vector<int> pred;

    raven::graph::dijsktra(
        theGraph,
        theEdgeWeight,
        rand() % theGraph.vertexCount(), // random start vertex
        dist,
        pred);
}

void allPaths()
{
    raven::graph::sGraphData gd;
    gd.g = theGraph;
    gd.edgeWeight.resize(gd.g.edgeCount(),1);
    gd.startName = gd.g.userName(
        rand() % theGraph.vertexCount()); // random start vertex
    gd.endName = gd.g.userName(
        rand() % theGraph.vertexCount()); // random start vertex

    allPaths( gd );
}

void ancestor_recurse(
    int v,
    int depth,
    std::set<int> &setAncestors)
{
    if (depth > 5)
        return;

    // loop over imediate ancestors
    for (int a : theGraph.adjacentIn(v))
    {
        // add to set of unique ancestors
        setAncestors.insert(a);

        // look for deeper ancestors
        ancestor_recurse(a, depth + 1, setAncestors);
    }
}

void ancestor()
{
#define REPEAT 1000

    // loop over vertices
    for (int r = 0; r < REPEAT; r++)
    {
        // time profiler
        raven::set::cRunWatch aWatcher("ancestor");

        // select random node
        int v = rand() % theGraph.vertexCount();

        // find unique ancestors
        std::set<int> setAncestors;
        ancestor_recurse(v, 0, setAncestors);

        // average value of ancestors
        double total = 0;
        for (int a : setAncestors)
        {
            total += atof(theGraph.rVertexAttr(a, 0).c_str());
        }
        double average = total / setAncestors.size();

        // store average in vertex attributes
        theGraph.wVertexAttr(
            v,
            {theGraph.rVertexAttr(v, 0),
             std::to_string(average)});

        // std::cout << v <<" "<< average << "\n";
    }
}

void help()
{
    std::cout << "\nSupported queries\n"
                 "add v1 v2 :     add link to graph\n"
                 "add random n :  add n random links to graph\n"
                 "read filepath : input graph links from file\n"
                 "display :       display links\n"
                 "paths :         find all paths from random vertex to all others ( Dijkstra )\n"
                 "allpaths :      find all paths between two random vertices ( Dijkstra )\n"
                 "cycles :        find cycles in graph\n"
                 "s2s :           find source to sink connections\n"
                 "gen n :         generate random graph with n vertices\n"
                 "euler n :       find euler path\n"
                 "span :          find spanning tree\n"
                 "help :          this help display\n\n";
}

main()
{
    raven::set::cRunWatch::Start();

    help();

    while (1)
    {
        auto q = userQuery();

        {
            raven::set::cRunWatch aWatcher("query");

            if (!q.size())
                continue;
            if (q[0] == "add")
                add(q);
            else if (q[0] == "display")
                display(q);
            else if (q[0] == "help")
                help();
            else if (q[0] == "read")
                read(q);
            else if (q[0] == "ancestor")
                ancestor();
            else if (q[0] == "cycles")
                cycles();
            else if (q[0] == "s2s")
                s2s();
            else if (q[0] == "gen")
                gen(q);
            else if (q[0] == "euler")
                raven::graph::euler(theGraph);
            else if (q[0] == "span")
                span();
            else if (q[0] == "paths")
                paths();
            else if (q[0] == "allpaths")
                allPaths();
            else if (q[0] == "cover")
                vertexCover(theGraph);
            else
                std::cout << "unreqonized query\n";
        }

        raven::set::cRunWatch::Report();
        std::cout << "\n";
        raven::set::cRunWatch::Clear();
    }

    return 0;
}
