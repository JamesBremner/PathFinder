#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"
#include "cGrid2D.h"

TEST(add)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    CHECK_EQUAL(2, g.edgeCount());

    g.clear();
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    CHECK_EQUAL(2, g.edgeCount());

    g.clear();
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");

    int nb = g.find("b");
    int nc = g.find("c");
    CHECK(nb != -1);
    CHECK(nc != -1);

    int ei = g.find(nb, nc);
    CHECK(ei != -1);

    g.clear();
    g.directed();
    int ab = g.add("a", "b");
    int ba = g.add("b", "a");
    CHECK_EQUAL(0, ab);
    CHECK_EQUAL(1, ba);

    g.clear();
    ab = g.add("a", "b");
    ba = g.add("b", "a");
    CHECK_EQUAL(0, ab);
    CHECK_EQUAL(1, ba);
}

TEST(bfs)
{
    // these tests are based on the example in https://stackoverflow.com/q/76954728/16582
    // note that here the station indices are zero-based
    // and both start and destination count in the path length

    raven::graph::sGraphData gd;
    gd.g.add("0", "1");
    gd.g.add("1", "2");
    gd.g.add("2", "3");
    gd.g.add("3", "4");
    gd.g.add("2", "5");
    gd.g.add("2", "6");
    gd.g.add("2", "7");
    gd.g.add("1", "8");
    gd.g.add("4", "6");

    gd.startName = "0";
    gd.endName = "4";
    auto path = bfsPath(gd);
    CHECK_EQUAL(5, path.size());

    gd.startName = "0";
    gd.endName = "1";
    path = bfsPath(gd);
    CHECK_EQUAL(2, path.size());

    gd.startName = "2";
    gd.endName = "5";
    path = bfsPath(gd);
    CHECK_EQUAL(2, path.size());
}

TEST(tarjan_gg3)
{
    raven::graph::sGraphData gd;
    gd.g.add("1", "2");
    gd.g.add("2", "3");
    gd.g.add("3", "4");
    gd.g.add("4", "5");
    gd.g.add("4", "6");
    gd.g.add("4", "7");

    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(3, vAP.size());
    CHECK_EQUAL("2", vAP[0]);
    CHECK_EQUAL("3", vAP[1]);
    CHECK_EQUAL("4", vAP[2]);
}

TEST(tarjan_so76904782)
{
    raven::graph::sGraphData gd;
    gd.g.add("0", "1");
    gd.g.add("1", "2");
    gd.g.add("2", "0");
    gd.g.add("1", "3");
    gd.g.add("3", "2");
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(0, vAP.size());
}

TEST(tarjan_gg1)
{
    raven::graph::sGraphData gd;
    // gd.g.directed();
    gd.g.add("1", "0");
    gd.g.add("0", "2");
    gd.g.add("2", "1");
    gd.g.add("0", "3");
    gd.g.add("3", "4");
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(2, vAP.size());
    CHECK_EQUAL("0", vAP[0]);
    CHECK_EQUAL("3", vAP[1]);
}

TEST(tarjan_gg2)
{
    raven::graph::sGraphData gd;
    gd.g.add("0", "1");
    gd.g.add("1", "2");
    gd.g.add("2", "3");
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(2, vAP.size());
    CHECK_EQUAL("1", vAP[0]);
    CHECK_EQUAL("2", vAP[1]);
}

TEST(AP_so76642739_undirected)
{
    raven::graph::sGraphData gd;
    gd.g.add("0", "2");
    gd.g.add("2", "1");
    gd.g.add("1", "0");
    gd.g.add("0", "4");
    gd.g.add("2", "3");
    gd.g.add("4", "5");
    gd.g.add("5", "6");
    gd.g.add("6", "3");
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(0, vAP.size());
}

TEST(AP_so76642739_undirected_2)
{
    // define edges in a different order
    raven::graph::sGraphData gd;
    gd.g.add("0", "4");
    gd.g.add("0", "2");
    gd.g.add("2", "1");
    gd.g.add("1", "0");
    gd.g.add("2", "3");
    gd.g.add("4", "5");
    gd.g.add("5", "6");
    gd.g.add("6", "3");
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(gd);
    CHECK_EQUAL(0, vAP.size());
}

TEST(AP_so76642739_directed)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("0", "2");
    gd.g.add("2", "1");
    gd.g.add("1", "0");
    gd.g.add("0", "4");
    gd.g.add("2", "3");
    gd.g.add("4", "5");
    gd.g.add("5", "6");
    gd.g.add("6", "3");
    raven::graph::cTarjan T;
    try
    {
        auto vAP = T.ArticulationPoints(gd);
        CHECK(false);
    }
    catch (...)
    {
    }
}

TEST(grid2graph1)
{
    cGrid2D grid;
    raven::graph::sGraphData gd;

    grid.setDim(7, 7);
    grid.addOrthoEdges();
    grid.addEdges2Graph(gd.g);
    gd.edgeWeight.resize(2 * gd.g.edgeCount(), 1);

    gd.startName = "c0r0";
    gd.endName = "c1r5";
    auto pa = path(gd).first;
    auto spa = gd.g.userName(pa);
    CHECK_EQUAL(7, spa.size());
}

TEST(allpaths)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.g.add("d", "c");
    gd.edgeWeight.resize(8, 1);
    gd.startName = "a";
    gd.endName = "c";

    auto act = allPaths(gd);

    CHECK_EQUAL(2, act.size());

    std::vector<std::string> expected1{"a", "d", "c"};
    CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        gd.g.userName(act[0]).begin()));

    std::vector<std::string> expected2{"a", "b", "c"};
    CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        gd.g.userName(act[1]).begin()));
}

TEST(probs)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("a", "c");
    gd.g.add("b", "c");
    gd.edgeWeight.resize(2, 0.5);
    gd.endName = "c";

    int act = 100 * probs(gd);

    CHECK_EQUAL(75, act);

    //     l u c 0.3
    // l u d 0.5
    // l c a 0.2
    // l c b 0.2
    // l d b 0.4
    // l a v 0.1
    // l b v 0.1
    // e v

    gd.g.clear();
    gd.g.directed();
    gd.edgeWeight.resize(7);
    gd.edgeWeight[gd.g.add("u", "c")] = 0.3;
    gd.edgeWeight[gd.g.add("u", "d")] = 0.5;
    gd.edgeWeight[gd.g.add("c", "a")] = 0.2;
    gd.edgeWeight[gd.g.add("c", "b")] = 0.2;
    gd.edgeWeight[gd.g.add("d", "b")] = 0.4;
    gd.edgeWeight[gd.g.add("a", "v")] = 0.1;
    gd.edgeWeight[gd.g.add("b", "v")] = 0.1;
    gd.endName = "v";

    act = 100 * probs(gd);
    CHECK_EQUAL(3, act);
}

TEST(salesBBmetric)
{
    raven::graph::sGraphData gd;
    gd.fname = "../dat/tspmetric.txt";
    readfile(gd);
    raven::graph::cTSP tpsbb(gd.g, gd.edgeWeight);
    auto path = gd.g.userName(tpsbb.calculate());
    std::vector<std::string> exp{"0", "1", "3", "2", "0"};
    CHECK_EQUAL(80, tpsbb.TotalPathEdgeWeight());
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        path.begin()));
}
TEST(salesBBnotmetric)
{
    raven::graph::sGraphData gd;
    gd.fname = "../dat/tspnotmetric.txt";
    readfile(gd);
    raven::graph::cTSP tpsbb(gd.g, gd.edgeWeight);
    auto path = gd.g.userName(tpsbb.calculate());
    std::vector<std::string> exp{"0", "1", "3", "2", "0"};
    CHECK_EQUAL(27, tpsbb.TotalPathEdgeWeight());
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        path.begin()));
}
TEST(salesmetric)
{
    raven::graph::sGraphData gd;
    gd.fname = "../dat/tspmetric.txt";
    readfile(gd);
    // raven::graph::cTourNodes tpsbb(gd.g, gd.edgeWeight);
    raven::graph::cTourNodes tourer;
    tourer.calculate(gd);
    auto path = gd.g.userName(tourer.getTour());

    std::vector<std::string> exp{"0", "3", "1", "2"};
    // CHECK_EQUAL(80, tpsbb.TotalPathEdgeWeight());
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        path.begin()));
}

TEST(cliques)
{
    raven::graph::sGraphData gd;
    gd.fname = "../dat/cliques.txt";
    readfile(gd);
    std::string results;
    cliques(gd.g, results);
    std::string expected("clique: 1 5 3 7 \nclique: 2 8 6 4 \n");
    CHECK_EQUAL(expected, results);
}

TEST(dfs_allpaths)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("0", "1");
    gd.g.add("0", "2");
    gd.g.add("0", "3");
    gd.g.add("2", "0");
    gd.g.add("2", "1");
    gd.g.add("1", "3");
    gd.startName = "2";
    gd.endName = "3";

    auto vpaths = dfs_allpaths(gd);

    CHECK_EQUAL(3, vpaths.size());
    std::vector<int> expected1{2, 1, 3};
    CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        vpaths[0].begin()));
    std::vector<int> expected2{2, 0, 3};
    CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        vpaths[1].begin()));
    std::vector<int> expected3{2, 0, 1, 3};
    CHECK(std::equal(
        expected3.begin(),
        expected3.end(),
        vpaths[2].begin()));
}

TEST(dfs_allpaths2)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.g.add("d", "c");
    gd.startName = "a";
    gd.endName = "c";

    auto act = dfs_allpaths(gd);

    CHECK_EQUAL(2, act.size());

    std::vector<std::string> expected1{"a", "d", "c"};
    CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        gd.g.userName(act[0]).begin()));

    std::vector<std::string> expected2{"a", "b", "c"};
    CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        gd.g.userName(act[1]).begin()));
}

TEST(vertexCover1)
{
    raven::graph::cGraph g;
    g.add("0", "2");
    g.add("2", "4");
    g.add("1", "4");
    g.add("3", "4");

    auto act = g.userName(vertexCover(g));
    std::vector<std::string> exp{"2", "4"};
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}
TEST(vertexCover2)
{
    raven::graph::cGraph g;
    g.add("1", "2");
    g.add("2", "3");
    g.add("4", "5");
    g.add("5", "6");
    g.add("6", "7");

    auto act = g.userName(vertexCover(g));
    std::vector<std::string> exp{"2", "5", "6"};
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}
TEST(vertexCover3)
{
    raven::graph::cGraph g;
    g.add("0", "1");
    g.add("0", "2");
    g.add("1", "2");
    g.add("1", "3");
    g.add("2", "4");
    g.add("3", "4");
    g.add("3", "5");

    auto act = g.userName(vertexCover(g));
    std::vector<std::string> exp{"1", "2", "3"};
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}
TEST(cycle0)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("c", "d");
    gd.g.add("d", "a");

    auto act = dfs_cycle_finder(gd);
    CHECK_EQUAL(1, act.size());
    CHECK_EQUAL(5, act[0].size());
}
TEST(cycle)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("d", "a");
    gd.g.add("c", "d");

    auto act = dfs_cycle_finder(gd);
    CHECK_EQUAL(1, act.size());
    CHECK_EQUAL(5, act[0].size());
}

TEST(cycle2)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("d", "a");
    gd.g.add("c", "d");
    gd.g.add("b", "e");
    gd.g.add("e", "f");
    gd.g.add("f", "g");
    gd.g.add("g", "e");

    auto act = dfs_cycle_finder(gd);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(5, act[0].size());
    CHECK_EQUAL(4, act[1].size());
}
TEST(cycle2undirected)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("d", "a");
    gd.g.add("c", "d");
    gd.g.add("b", "e");
    gd.g.add("e", "f");
    gd.g.add("f", "g");
    gd.g.add("g", "e");

    auto act = dfs_cycle_finder(gd);
    CHECK_EQUAL(5, act[0].size());
    CHECK_EQUAL(4, act[1].size());
}

TEST(cycle2notfullyconnected)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("d", "a");
    gd.g.add("c", "d");
    gd.g.add("e", "b"); // directed link makes a,b,c,d reachable from e,f,g but not vice versa
    gd.g.add("e", "f");
    gd.g.add("f", "g");
    gd.g.add("g", "e");

    auto act = dfs_cycle_finder(gd);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(5, act[0].size());
    CHECK_EQUAL(4, act[1].size());
}

TEST(tourNodes)
{
    raven::graph::sGraphData gd;
    gd.g.directed(false);
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.edgeWeight = {1, 1, 1, 1, 1, 1};
    raven::graph::cTourNodes tourer;

    tourer.calculate(gd);

    CHECK_EQUAL(0, tourer.unvisitedCount());
    CHECK_EQUAL(0, tourer.revisitedCount());

    auto tour = tourer.getTour();
    CHECK_EQUAL(4, tour.size());
    std::vector<std::string> expected{"c", "b", "a", "d"};
    auto actual = gd.g.userName(tour);
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        actual.begin()));
}

TEST(flows)
{
    raven::graph::sGraphData gd;

    gd.g.directed();
    gd.g.add("a", "b");
    gd.edgeWeight.resize(1, 7);
    gd.startName = "a";
    gd.endName = "b";

    std::vector<int> vEdgeFlow;
    double f = flows(gd,
                     vEdgeFlow);

    CHECK_EQUAL(7.0, f);
}

TEST(removeLink)
{

    raven::graph::cGraph g;

    // directed

    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");

    g.remove("b", "c");

    CHECK_EQUAL(2, g.edgeCount());
    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= 0);

    CHECK(g.find(g.find("a"), g.find("d")) >= 0);

    g.remove("a", "d");

    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= -1);

    // undirected

    g.clear();
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    CHECK_EQUAL(3, g.edgeCount());

    g.remove("b", "c");

    CHECK_EQUAL(2, g.edgeCount());
    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= 0);

    int ei = g.find(g.find("a"), g.find("d"));
    CHECK(ei >= 0);

    g.remove(ei);

    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= -1);
}

TEST(edgebyindex)
{
    raven::graph::cGraph g;
    g.add(1, 2);
    g.add(2, 3);
    CHECK_EQUAL(2, g.edgeCount());
    std::vector<std::string> exp{"V1", "V2", "V3"};
    auto act = g.userName({1, 2, 3});
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}

TEST(attributes)
{
    raven::graph::cGraph g;
    int v1 = g.add("a");
    int v2 = g.add("b");
    int ei = g.add("a", "b");
    g.wVertexAttr(v1, {"10", "11", "12"});
    g.wVertexAttr(v2, {"20", "21", "22"});

    CHECK_EQUAL("11", g.rVertexAttr(v1, 1));
    CHECK_EQUAL("22", g.rVertexAttr(v2, 2));
}

TEST(adjacent)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");

    auto res = g.userName(g.adjacentOut(g.find("a")));
    std::vector<std::string>
        aexp{"b", "d"};
    CHECK(std::equal(
        aexp.begin(),
        aexp.end(),
        res.begin()));

    res = g.userName(g.adjacentOut(g.find("b")));
    std::vector<std::string>
        bexp{"a", "c"};
    CHECK(std::equal(
        bexp.begin(),
        bexp.end(),
        res.begin()));
}
TEST(dijsktra)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.edgeWeight.resize(3, 1);
    gd.startName = "a";
    gd.endName = "c";

    std::vector<std::string> expected{"a", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        gd.g.userName(path(gd).first).begin()));
}

TEST(spanningTree)
{
    raven::graph::sGraphData gd;
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.edgeWeight.resize(3, 1);
    gd.startName = "a";

    std::vector<std::pair<int, int>> expected{
        {0, 3},
        {0, 1},
        {1, 2}};

    auto res = spanningTree(gd).edgeList();

    CHECK_EQUAL(3, res.size());

    CHECK(std::equal(
        expected.begin(), expected.end(),
        res.begin()));
}

TEST(dfs)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    g.add("d", "e");

    std::vector<int> visited;
    dfs(g, g.find("a"),
        [&](int v)
        {
            visited.push_back(v);
            return true;
        });
    std::vector<std::string> expected{"a", "d", "e", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(visited).begin()));
}

TEST(tourNodes2)
{
    raven::graph::sGraphData gd;
    gd.g.directed(false);
    gd.g.add("a", "b");
    gd.g.add("b", "c");
    gd.g.add("a", "d");
    gd.g.add("c", "d");
    gd.edgeWeight.resize(4, 1);
    raven::graph::cTourNodes tourer;

    tourer.calculate(gd);
    auto tour = tourer.getTour();

    std::vector<std::string> expected{"a", "b", "c", "d"};
    auto actual = gd.g.userName(tour);
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        actual.begin()));
}

TEST(sourceToSink)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("a", "b");
    g.add("b", "e");
    g.add("b", "d");
    g.add("c", "d");
    std::vector<double> edgeWeight(4, 1);

    auto res = sourceToSink(g, edgeWeight);

    auto dbg = g.userName(res[1]);

    CHECK_EQUAL(2, res.size());
    std::vector<std::string> exp1{"a", "e", "d"};
    CHECK(std::equal(
        exp1.begin(),
        exp1.end(),
        g.userName(res[0]).begin()));
    std::vector<std::string> exp2{"c", "d"};
    CHECK(std::equal(
        exp2.begin(),
        exp2.end(),
        g.userName(res[1]).begin()));
}

TEST(cGrid2D)
{
    cGrid2D g;
    g.extDim(2, 1);
    CHECK_EQUAL(5, g.index(2, 1));
    CHECK_EQUAL(4, g.index(1, 1));
}

TEST(alloc)
{
    raven::graph::sGraphData gd;
    gd.g.directed();
    gd.g.add("child1", "chore1");
    gd.g.add("child1", "chore2");
    gd.g.add("child1", "chore3");
    gd.g.add("child2", "chore1");
    gd.g.add("child2", "chore2");
    gd.g.add("child2", "chore3");

    auto act = alloc(gd);

    std::vector<std::string> exp{"child1", "chore2", "child2", "chore3"};
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}

TEST(Euler2)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("0", "1");
    g.add("0", "6");
    g.add("1", "2");
    g.add("2", "0");
    g.add("2", "3");
    g.add("3", "4");
    g.add("4", "2");
    g.add("4", "5");
    g.add("5", "0");
    g.add("6", "4");

    auto act = euler(g);

    std::vector<std::string> exp1{"0", "1", "2", "0", "6", "4", "2", "3", "4", "5", "0"};

    auto sact = g.userName(act);

    CHECK(std::equal(
        exp1.begin(),
        exp1.end(),
        sact.begin()));
}

TEST(Euler)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("c", "a");

    auto act = euler(g);

    CHECK_EQUAL(4, act.size());

    std::vector<std::string> exp1{"a", "b", "c", "a"};

    auto sact = g.userName(act);

    CHECK(std::equal(
        exp1.begin(),
        exp1.end(),
        sact.begin()));
}

#include "chemTest.cpp"

main()
{
    return raven::set::UnitTest::RunAllTests();
}