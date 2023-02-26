#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"



TEST(findorAdd)
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    CHECK_EQUAL(2, g.edgeCount());

    g.clear();
    g.directed();
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    CHECK_EQUAL(2, g.edgeCount());
}

TEST(edgebyindex)
{
    raven::cGraphData g;
    g.add(1, 2, "1");
    g.add(2, 3, "1");
    CHECK_EQUAL(2, g.edgeCount());
    std::vector<std::string> exp{"1", "2", "3"};
    auto act = g.userName({1, 2, 3});
    CHECK(std::equal(
        exp.begin(),
        exp.end(),
        act.begin()));
}

TEST( attributes)
{
    raven::cGraphData g;
    int v1 = g.add("a");
    int v2 = g.add("b");
    int ei = g.add("a","b");
    g.wVertexAttr(v1,{"10","11","12"});
    g.wVertexAttr(v2,{"20","21","22"});
    g.wEdgeAttr(ei,{"50","51","52"});

    CHECK_EQUAL(11.0,g.rVertexAttr(v1,1));
    CHECK_EQUAL(22.0,g.rVertexAttr(v2,2));
    CHECK_EQUAL(50.0,g.rEdgeAttr(ei,0));
}

TEST( removeLink )
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    g.remove("b","c");

    CHECK_EQUAL( 2, g.edgeCount());
}

TEST(adjacent)
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    auto res = g.adjacentOut("a");
    std::vector<std::string>
        aexp{"b", "d"};
    CHECK(std::equal(
        aexp.begin(),
        aexp.end(),
        res.begin()));

    res = g.adjacentOut("b");
    std::vector<std::string>
        bexp{"a", "c"};
    CHECK(std::equal(
        bexp.begin(),
        bexp.end(),
        res.begin()));
}
TEST(dijsktra)
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    std::vector<std::string> expected{"a", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(path(g, "a", "c").first).begin()));
}

TEST(spanningTree)
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");
    std::string expected(
        "l b a 1\n"
        "l b c 1\n"
        "l a d 1\n");

    CHECK_EQUAL(expected, spanningTree(g, "a").text());
}

TEST(dfs)
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    std::vector<int> visited;
    dfs(g, "a",
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
    raven::cGraphData g;
    g.directed(false);
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");
    raven::cTourNodes tourer(g);
    tourer.calculate();
    auto tour = tourer.getTour();

    std::vector<std::string> expected{"c", "b", "a", "d"};
    auto actual = g.userName(tour);
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        actual.begin()));
}

TEST(tourNodes2)
{
    raven::cGraphData g;
    g.directed(false);
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");
    g.findorAdd("c", "d", "1");
    raven::cTourNodes tourer(g);
    tourer.calculate();
    auto tour = tourer.getTour();

    std::vector<std::string> expected{"b", "c", "d", "a"};
    auto actual = g.userName(tour);
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        actual.begin()));
}

TEST(cycle)
{
    raven::cGraphData g;
    g.directed();
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("d", "a", "1");
    g.findorAdd("c", "d", "1");

    auto act = dfs_cycle_finder(g,"a");
    CHECK_EQUAL(1,act.size());
    CHECK_EQUAL(5,act[0].size());
}
TEST(cycle2)
{
    raven::cGraphData g;
    g.directed();
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("d", "a", "1");
    g.findorAdd("c", "d", "1");
    g.findorAdd("b", "e", "1");
    g.findorAdd("e", "f", "1");
    g.findorAdd("f", "g", "1");
    g.findorAdd("g", "e", "1");
    

    auto act = dfs_cycle_finder(g,"a");
    CHECK_EQUAL(2,act.size());
    CHECK_EQUAL(4,act[0].size());
}

TEST( flows )
{
    raven::cGraphData g;
    g.findorAdd("a", "b", "7");\
    double f = flows(
        g, "a", "b" );
    CHECK_EQUAL(7.0,f);
}

main()
{
    std::cout << "run all tests\n";
    return raven::set::UnitTest::RunAllTests();
}