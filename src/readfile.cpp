#include <fstream>
#include "cGraphData.h"
#include "cpathfinderGUI.h"

static void readCostedLinks(
    cGraphData &g,
    std::ifstream &ifs)
{
    std::string stype, sn1, sn2, scost;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'l':
            ifs >> sn1 >> sn2 >> scost;
            g.findorAdd(sn1, sn2, scost);
            break;
        case 's':
            ifs >> sn1;
            theGUI.start(sn1);
            break;
        case 'e':
            ifs >> sn1;
            theGUI.end(sn1);
            break;
        }

        ifs >> stype;
    }
}

graph_calc readfile(
    cGraphData &g,
    const std::string &fname)
{
    graph_calc option = graph_calc::none;
    g.clear();
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open input file");

    std::string form, calc;
    ifs >> form >> calc;
    if (form != "format")
        throw std::runtime_error(
            "Bad input format");

    if (calc.find("cost") != -1)
    {
        option = graph_calc::cost;
        readCostedLinks( g, ifs );
    }

    else if (calc.find("cycle") != -1)
    {
        option = graph_calc::cycle;
        std::string sn1, sn2;
        ifs >> sn1 >> sn2;

        while (ifs.good())
        {
            g.findorAdd(sn1, sn2, "1");
            ifs >> sn1 >> sn2;
        }
    }

    else if (calc.find("astar") != -1)
    {
        option = graph_calc::astar;
        std::string sn1, sn2;
        ifs >> sn1 >> sn2;

        while (ifs.good())
        {
            g.findorAdd(sn1, sn2, "1");
            ifs >> sn1 >> sn2;
        }
    }

    else if (calc.find("tour") != -1)
    {
        option = graph_calc::tour;
        readCostedLinks( g, ifs );
    }

    else
        throw std::runtime_error(
            "bad calculation type ");

    return option;
}
