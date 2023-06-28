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
    std::vector<int> bondtype(6,1);

    auto act = SMILES(ingraph,bondtype);
    CHECK_EQUAL("C1CCCCC1",act);
}

TEST(readSMILES)
{
    raven::graph::cGraph g;
    std::vector<int> bondtype;
    readSMILES(g,bondtype,"CCO");

    std::map<std::string, int> mpCount;
    for (int vi = 0; vi < g.vertexCount(); vi++)
    {
        mpCount[g.rVertexAttr(vi, 0)]++;
    }
    CHECK_EQUAL(2, mpCount["C"]);
    CHECK_EQUAL(1, mpCount["O"]);

//     readSMILES(g,"C=CN");
//     chemViz(g);
 }

TEST(readGraph)
{
    raven::graph::cGraph ingraph;
    std::vector<int> bondtype(6,1);
    ingraph.add("C1", "H1");
    ingraph.add("C1", "H2");
    bondtype[ingraph.add("C1", "C2")] = 2; // double bond
    ingraph.add("C2", "H5");
    ingraph.add("C2", "O1");
    ingraph.add("C1", "H6");

    // TODO
    //auto act = SMILES(ingraph,bondtype,bondtype);
    // CHECK_EQUAL("C=CO",);
}