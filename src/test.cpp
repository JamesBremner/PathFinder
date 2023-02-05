#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"

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
        "b - c\n"
        "a - b\n"
        "a - d\n");
    CHECK_EQUAL(expected, spanningTree(g, "a").text());
}

main()
{
    return raven::set::UnitTest::RunAllTests();
}