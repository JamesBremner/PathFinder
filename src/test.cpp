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
        "b - a\n"
        "b - c\n"
        "a - d\n");
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
        });
    std::vector<std::string> expected{"a", "d", "b", "c"};
    CHECK(std::equal(
        expected.begin(),
        expected.end(),
        g.userName(visited).begin()));
}

main()
{
    return raven::set::UnitTest::RunAllTests();
}