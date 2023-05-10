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

TEST( cliques )
{
    raven::graph::cGraph g;
    readfile( g, "../dat/cliques.txt");
    std::string results;
    cliques( g, results );
    std::string expected("clique: 1 5 3 7 \nclique: 2 8 6 4 \n");
    CHECK_EQUAL( expected, results);
}

TEST( dfs_allpaths )
{
    raven::graph::cGraph g;
    g.directed();
    g.add("0", "1");
    g.add("0", "2");
    g.add("0", "3");
    g.add("2", "0");
    g.add("2", "1");
    g.add("1", "3");

    auto vpaths = dfs_allpaths( 
        g,
        g.find("2"),
        g.find("3"));

    CHECK_EQUAL( 3, vpaths.size() );
    std::vector<int> expected1 {2,1,3};
        CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        vpaths[0].begin()));
    std::vector<int> expected2 {2,0,3};
        CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        vpaths[1].begin()));
    std::vector<int> expected3 {2,0,1,3};
        CHECK(std::equal(
        expected3.begin(),
        expected3.end(),
        vpaths[2].begin()));
}

TEST(dfs_allpaths2)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    g.add("d", "c");

    auto act = dfs_allpaths(g, g.find("a"), g.find("c"));

    CHECK_EQUAL(2, act.size());

    std::vector<std::string> expected1{"a", "d", "c"};
    CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        g.userName(act[0]).begin()));

    std::vector<std::string> expected2{"a", "b", "c"};
    CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        g.userName(act[1]).begin()));
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
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("c", "d");
    g.add("d", "a");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(1, act.size());
    CHECK_EQUAL(5, act[0].size());
}
TEST(cycle)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("d", "a");
    g.add("c", "d");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(1, act.size());
    CHECK_EQUAL(5, act[0].size());
}

TEST(cycle2)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("d", "a");
    g.add("c", "d");
    g.add("b", "e");
    g.add("e", "f");
    g.add("f", "g");
    g.add("g", "e");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(4, act[0].size());
    CHECK_EQUAL(5, act[1].size());
}
TEST(cycle2undirected)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("d", "a");
    g.add("c", "d");
    g.add("b", "e");
    g.add("e", "f");
    g.add("f", "g");
    g.add("g", "e");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(5, act[0].size());
    CHECK_EQUAL(4, act[1].size());
}

TEST(cycle2notfullyconnected)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("a", "b");
    g.add("b", "c");
    g.add("d", "a");
    g.add("c", "d");
    g.add("e", "b"); // directed link makes a,b,c,d reachable from e,f,g but not vice versa
    g.add("e", "f");
    g.add("f", "g");
    g.add("g", "e");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(5, act[0].size());
    CHECK_EQUAL(4, act[1].size());
}

TEST(tourNodes)
{
    raven::graph::cGraph g;
    g.directed(false);
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    raven::graph::cTourNodes tourer;

    tourer.calculate(g);

    CHECK_EQUAL(0, tourer.unvisitedCount());
    CHECK_EQUAL(0, tourer.revisitedCount());

    auto tour = tourer.getTour();
    CHECK_EQUAL(4, tour.size());
    // std::vector<std::string> expected{"c", "b", "d", "a"};
    // auto actual = g.userName(tour);
    // CHECK(std::equal(
    //     expected.begin(),
    //     expected.end(),
    //     actual.begin()));
}

TEST(flows)
{
    raven::graph::cGraph g;
    g.directed();

    g.wEdgeAttr(g.add("a", "b"), {"7"});

    std::vector<int> vEdgeFlow;
    double f = flows(
        g, g.find("a"), g.find("b"),
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

    //     ei = g.find(g.find("a"), g.find("d"));
    //     CHECK(ei >= 0);

    //     g.remove(ei);

    //     CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    //     CHECK(g.find(g.find("b"), g.find("b")) == -1);
    //     CHECK(g.find(g.find("a"), g.find("d")) >= -1);
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
    g.wEdgeAttr(ei, {"50", "51", "52"});

    CHECK_EQUAL("11", g.rVertexAttr(v1, 1));
    CHECK_EQUAL("22", g.rVertexAttr(v2, 2));
    CHECK_EQUAL("50", g.rEdgeAttr(ei, 0));
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
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");

    std::vector<std::string> expected{"a", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(path(g, "a", "c").first).begin()));
}
TEST(allpaths)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    g.add("d", "c");

    auto act = allPaths(g, g.find("a"), g.find("c"));

    CHECK_EQUAL(2, act.size());

    std::vector<std::string> expected1{"a", "d", "c"};
    CHECK(std::equal(
        expected1.begin(),
        expected1.end(),
        g.userName(act[0]).begin()));

    std::vector<std::string> expected2{"a", "b", "c"};
    CHECK(std::equal(
        expected2.begin(),
        expected2.end(),
        g.userName(act[1]).begin()));
}

TEST(spanningTree)
{
    raven::graph::cGraph g;
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");

    std::vector<std::pair<int, int>> expected{
        {0, 1},
        {0, 3},
        {1, 2}};

    auto res = spanningTree(g, "a").edgeList();

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
    raven::graph::cGraph g;
    g.directed(false);
    g.add("a", "b");
    g.add("b", "c");
    g.add("a", "d");
    g.add("c", "d");
    raven::graph::cTourNodes tourer;

    tourer.calculate(g);
    auto tour = tourer.getTour();

    std::vector<std::string> expected{"a", "d", "c", "b"};
    auto actual = g.userName(tour);
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

    auto res = sourceToSink(g);

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

TEST(probs)
{
    raven::graph::cGraph g;
    g.directed();
    int e = g.add("a", "c");
    g.wEdgeAttr(e, {"0.5"});
    e = g.add("b", "c");
    g.wEdgeAttr(e, {"0.5"});
    int act = 100 * probs(
                        g,
                        g.find("c"));

    CHECK_EQUAL(75, act);

    //     l u c 0.3
    // l u d 0.5
    // l c a 0.2
    // l c b 0.2
    // l d b 0.4
    // l a v 0.1
    // l b v 0.1
    // e v

    g.clear();
    g.directed();
    e = g.add("u", "c");
    g.wEdgeAttr(e, {"0.3"});
    e = g.add("u", "d");
    g.wEdgeAttr(e, {"0.5"});
    e = g.add("c", "a");
    g.wEdgeAttr(e, {"0.2"});
    e = g.add("c", "b");
    g.wEdgeAttr(e, {"0.2"});
    e = g.add("d", "b");
    g.wEdgeAttr(e, {"0.4"});
    e = g.add("a", "v");
    g.wEdgeAttr(e, {"0.1"});
    e = g.add("b", "v");
    g.wEdgeAttr(e, {"0.1"});

    act = 100 * probs(
                    g,
                    g.find("v"));
    CHECK_EQUAL(3, act);
}

TEST(alloc)
{
    raven::graph::cGraph g;
    g.directed();
    g.add("child1", "chore1");
    g.add("child1", "chore2");
    g.add("child1", "chore3");
    g.add("child2", "chore1");
    g.add("child2", "chore2");
    g.add("child2", "chore3");

    auto act = alloc(g);

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