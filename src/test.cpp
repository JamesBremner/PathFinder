#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"

TEST(findorAdd)
{
    cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    CHECK_EQUAL(2, g.edgeCount());
}
TEST(dijsktra)
{
    cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    std::vector<std::string> expected{"a", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(path(g, "a", "c")).begin()));
}

TEST(spanningTree)
{
    cGraphData g;
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
    cGraphData g;
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
    cGraphData g;
    g.findorAdd("a", "b", "1");
    g.findorAdd("b", "c", "1");
    g.findorAdd("a", "d", "1");

    auto tour = tourNodes(g);

    std::vector<std::string> expected{"b", "c", "d", "a"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(tour).begin()));
}

main()
{
    return raven::set::UnitTest::RunAllTests();
}