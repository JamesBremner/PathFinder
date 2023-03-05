#include <string>
#include <iostream>
#include "cutest.h"
#include "GraphTheory.h"

TEST(sourceToSink)
{
    raven::graph::cGraph g;
    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "e");
    g.findorAdd("b", "d");
    g.findorAdd("c", "d");

    auto res = sourceToSink( g );

    auto dbg = g.userName(res[1]);

    CHECK_EQUAL(2,res.size());
    std::vector<std::string> exp1 { "a","e","d"};
    CHECK(std::equal(
        exp1.begin(),
        exp1.end(),
        g.userName(res[0]).begin() ));
    std::vector<std::string> exp2 { "c","d"};
    CHECK(std::equal(
        exp2.begin(),
        exp2.end(),
        g.userName(res[1]).begin() ));
}

TEST(removeLink)
{
    raven::graph::cGraph g;

    // directed

    g.directed();
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

    g.remove(g.find(g.find("b"), g.find("c")));

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

    ei = g.find(g.find("a"), g.find("d"));
    CHECK(ei >= 0);

    g.remove(ei);

    CHECK(g.find(g.find("a"), g.find("b")) >= 0);
    CHECK(g.find(g.find("b"), g.find("b")) == -1);
    CHECK(g.find(g.find("a"), g.find("d")) >= -1);

}

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
}

TEST(edgebyindex)
{
    raven::graph::cGraph g;
    g.add(1, 2);
    g.add(2, 3);
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
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

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
    std::string expected(
        "l b a\n"
        "l b c\n"
        "l a d\n");

    CHECK_EQUAL(expected, spanningTree(g, "a").text());
}

TEST(dfs)
{
    raven::graph::cGraph g;
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");

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
    raven::graph::cGraph g;
    g.directed(false);
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");
    raven::graph::cTourNodes tourer(g);
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
    raven::graph::cGraph g;
    g.directed(false);
    g.findorAdd("a", "b");
    g.findorAdd("b", "c");
    g.findorAdd("a", "d");
    g.findorAdd("c", "d");
    raven::graph::cTourNodes tourer(g);
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

TEST(flows)
{
    raven::graph::cGraph g;
    g.directed(false);
    g.wEdgeAttr(g.findorAdd("a", "b"), {"7"});
    g.wEdgeAttr(g.findorAdd("b", "a"), {"7"});
    double f = flows(
        g, "a", "b");
    CHECK_EQUAL(7.0, f);
}

main()
{
    std::cout << "run all tests\n";
    return raven::set::UnitTest::RunAllTests();
}