#include "chemistry.h"

TEST(cyclohexane)
{
    raven::graph::cGraph ingraph;
    ingraph.add("C1", "C2");
    ingraph.add("C2", "C3");
    ingraph.add("C3", "C4");
    ingraph.add("C4", "C5");
    ingraph.add("C5", "C6");
    ingraph.add("C6", "C1");


    auto act = SMILES(ingraph);
    CHECK_EQUAL("C1CCCCC1",act);
}

TEST(readSMILES)
{
    raven::graph::cGraph g;
    readSMILES(g,"CCO");

    std::map<std::string, int> mpCount;
    for (int vi = 0; vi < vertexCount(); vi++)
    {
        mpCount[rVertexAttr(vi, 0)]++;
    }
    CHECK_EQUAL(2, mp["C"]);
    CHECK_EQUAL(1, mp["O"]);

//     readSMILES(g,"C=CN");
//     chemViz(g);
 }

TEST(readGraph)
{
    raven::graph::cGraph ingraph;
    ingraph.add("C1", "H1");
    ingraph.add("C1", "H2");
    ingraph.wEdgeAttr(
        ingraph.add("C1", "C2"),
        {"2"}); // double bond
    ingraph.add("C2", "H5");
    ingraph.add("C2", "O1");
    ingraph.add("C1", "H6");

    auto act = SMILES(ingraph);
    // CHECK_EQUAL("C=CO",);
}