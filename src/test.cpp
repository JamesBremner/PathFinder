#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"



TEST(findorAdd)
{
    raven::graph::cGraph g;
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    CHECK_EQUAL(2, g.edgeCount());

    g.clear();
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    CHECK_EQUAL(2, g.edgeCount());

    g.clear();
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

    int nb = g.find("b");
    int nc = g.find("c");
    CHECK(nb != -1 );
    CHECK(nc != -1 );

    int ei = g.find(nb,nc);
    CHECK(ei!=-1);

    g.clear();
    g.directed();
    int ab = g.findorAdd("a", "b");
    int ba = g.findorAdd("b", "a");
    CHECK_EQUAL(0,ab);
    CHECK_EQUAL(1,ba);

    g.clear();
     ab = g.findorAdd("a", "b");
     ba = g.findorAdd("b", "a");
    CHECK_EQUAL(0,ab);
    CHECK_EQUAL(1,ba);

}


TEST(flows)
{
    raven::graph::cGraph g;

    g.wEdgeAttr(g.findorAdd("a", "b"), {"7"});

    double f = flows(
        g, "a", "b");

    CHECK_EQUAL(7.0, f);
}

TEST(removeLink)
{

    raven::graph::cGraph g;

    // directed

    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

    int nb = g.find("b");
    int nc = g.find("c");
    int ei = g.find(nb,nc);

   // g.remove(g.find(g.find("b"), g.find("c")));
   g.remove(ei);

    CHECK_EQUAL(2, g.edgeCount());
    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= 0);

    ei = g.find(g.find("a"), g.find("d"));
    CHECK(ei >= 0);

    g.remove(ei);

    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= -1);

    // undirected

    g.clear();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");
    CHECK_EQUAL(3, g.edgeCount());

    g.remove(g.find(g.find("b"), g.find("c")));

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
    g.findorAdd(1, 2);
    g.findorAdd(2, 3);
    CHECK_EQUAL(2, g.edgeCount());
    std::vector<std::string> exp{"1", "2", "3"};
    auto act = g.userName({1, 2, 3});
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}

TEST(attributes)
{
    // raven::graph::cGraph g;
    // int v1 = g.add("a");
    // int v2 = g.add("b");
    // int ei = g.add("a", "b");
    // g.wVertexAttr(v1, {"10", "11", "12"});
    // g.wVertexAttr(v2, {"20", "21", "22"});
    // g.wEdgeAttr(ei, {"50", "51", "52"});

    // CHECK_EQUAL("11", g.rVertexAttr(v1, 1));
    // CHECK_EQUAL("22", g.rVertexAttr(v2, 2));
    // CHECK_EQUAL("50", g.rEdgeAttr(ei, 0));
}

TEST(adjacent)
{
    raven::graph::cGraph g;
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

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
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

    std::vector<std::string> expected{"a", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(path(g, "a", "c").first).begin()));
}

TEST(spanningTree)
{
    raven::graph::cGraph g;
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

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
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

    std::vector<int> visited;
    dfs(g, g.find("a"),
        [&](int v)
        {
            visited.push_back(v);
            return true;
        });
    std::vector<std::string> expected{"a", "d", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(visited).begin()));
}

TEST(tourNodes)
{
    raven::graph::cGraph g;
    g.directed(false);
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");
    raven::graph::cTourNodes tourer;

    tourer.calculate(g);

    CHECK_EQUAL(0,tourer.unvisitedCount());
    CHECK_EQUAL(0,tourer.revisitedCount());

    auto tour = tourer.getTour();
    std::vector<std::string> expected{"d", "a", "b", "c"};
    auto actual = g.userName(tour);
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        actual.begin()));
}

TEST(tourNodes2)
{
    raven::graph::cGraph g;
    g.directed(false);
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");
    g.findorAdd("c", "d");
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

TEST(cycle)
{
    raven::graph::cGraph g;
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("d", "a");
    g.findorAdd("c", "d");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(1, act.size());
    CHECK_EQUAL(5, act[0].size());
}
TEST(cycle2)
{
    raven::graph::cGraph g;
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("d", "a");
    g.findorAdd("c", "d");
    g.findorAdd("b", "e");
    g.findorAdd("e", "f");
    g.findorAdd("f", "g");
    g.findorAdd("g", "e");

    auto act = dfs_cycle_finder(g);
    CHECK_EQUAL(2, act.size());
    CHECK_EQUAL(4, act[0].size());
}



TEST(sourceToSink)
{
    raven::graph::cGraph g;
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "e");
    g.findorAdd("b", "d");
    g.findorAdd("c", "d");

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

main()
{
    return raven::set::UnitTest::RunAllTests();
}