#include <fstream>
#include "cGraphData.h"
#include "cpathfinderGUI.h"

static void readSales(
    raven::cGraphData &g,
    std::ifstream &ifs)
{
    g.clear();

    enum class eInput
    {
        none,
        city,
        link,
    } inputType = eInput::none;

    std::string stype, sn1, sn2, scost, name, x, y;

    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'c':
            if (inputType == eInput::link)
                throw std::runtime_error(
                    "Cannot mix cities and links");
            inputType = eInput::city;
            ifs >> x >> y >> name;
            g.wVertexAttr(g.add(name), {x, y});
            break;

        case 'l':
            if (inputType == eInput::city)
                throw std::runtime_error(
                    "Cannot mix cities and links");
            inputType = eInput::link;
            ifs >> sn1 >> sn2 >> scost;
            g.findorAdd(sn1, sn2, scost);
            break;
        }

        ifs >> stype;
    }
}

static void readCostedLinks(
    raven::cGraphData &g,
    std::ifstream &ifs)
{
    g.clear();

    std::string stype, sn1, sn2, scost;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'g':
            if (g.vertexCount())
                throw std::runtime_error(
                    "g ( graph mode ) must be second line");
            g.directed();
            break;
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
        case 'c':
            ifs >> sn1;
        }

        ifs >> stype;
    }
}
static void readUncostedLinks(
    raven::cGraphData &g,
    std::ifstream &ifs)
{
    g.clear();

    std::string stype, sn1, sn2;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'g':
            if (g.vertexCount())
                throw std::runtime_error(
                    "g ( graph mode ) must be second line");
            g.directed();
            break;
        case 'l':
            ifs >> sn1 >> sn2;
            g.findorAdd(sn1, sn2, "1");
            break;
        case 's':
            ifs >> sn1;
            theGUI.start(sn1);
            break;
        case 'e':
            ifs >> sn1;
            theGUI.end(sn1);
            break;
        case 'c':
            ifs >> sn1;
        }

        ifs >> stype;
    }
}
static void readObstacles(
    raven::cGraphData &g,
    std::ifstream &ifs)
{
    std::string stype, sn1, sn2, scost, sx, sy;
    int ie;

    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'l':
            ifs >> sn1 >> sn2 >> scost;
            ie = g.findorAdd(sn1, sn2, scost);
            g.wEdgeAttr(ie, {sx, sy});
            break;
        }

        ifs >> stype;
    }
}
static void readCycle(
    raven::cGraphData &g,
    std::ifstream &ifs)
{
    g.clear();
    g.directed();
    std::string stype, sn1, sn2;
    ifs >> stype;

    while (ifs.good())
    {
        if (stype[0] == 'l')
        {
            ifs >> sn1 >> sn2;
            g.findorAdd(sn1, sn2, "1");
        }
        else if (stype[0] == 's')
        {
            ifs >> sn1;
            theGUI.start(sn1);
        }
        ifs >> stype;
    }
}


graph_calc readfile(
    raven::cGraphData &g,
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
        readCostedLinks(g, ifs);
    }

    else if (calc.find("cycle") != -1)
    {
        option = graph_calc::cycle;
        readCycle(g, ifs);
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
        readCostedLinks(g, ifs);
    }
    else if (calc.find("obs") != -1)
    {
        option = graph_calc::obs;
        readObstacles(g, ifs);
    }
    else if (calc.find("spans") != -1)
    {
        option = graph_calc::spans;
        readCostedLinks(g, ifs);
    }
    else if (calc.find("sales") != -1)
    {
        option = graph_calc::sales;
        readSales(g, ifs);
    }
    else if (calc.find("cliques") != -1)
    {
        option = graph_calc::cliques;
        readUncostedLinks(g, ifs);
    }
    else if (calc.find("flows") != -1)
    {
        option = graph_calc::flows;
        readCostedLinks(g, ifs);
    }
    else
        throw std::runtime_error(
            "bad calculation type ");

    return option;
}
