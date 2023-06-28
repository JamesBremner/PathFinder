#include <fstream>
#include "cGraph.h"
#include "GraphTheory.h"

static void readSales(
    raven::graph::cGraph &g,
    std::vector<double>& edgeWeight,
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
            g.add(sn1, sn2);
            edgeWeight.push_back(atof(scost.c_str()));
            break;
        }

        ifs >> stype;
    }
}

static void readCostedLinks(
    raven::graph::cGraph &g,
    std::vector<double>& edgeWeight,
    std::ifstream &ifs)
{
    g.clear();

    std::string stype, sn1, sn2, scost, directed, same;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'g':
            if (g.vertexCount())
                throw std::runtime_error(
                    "g ( graph mode ) must be second line");
            ifs >> directed >> same;
            if( directed == "1")
                g.directed();
            break;
        case 'l':
            ifs >> sn1 >> sn2 >> scost;
            g.add(sn1, sn2);
            edgeWeight.push_back(atof(scost.c_str()));
            if( same == "1") {
                edgeWeight.push_back(atof(scost.c_str()));
            }
            break;
        case 's':
            ifs >> sn1;
            g.startName(sn1);
            break;
        case 'e':
            ifs >> sn1;
            g.endName(sn1);
            break;
        case 'c':
            ifs >> sn1;
        }

        ifs >> stype;
    }
}
static void readUncostedLinks(
    raven::graph::cGraph &g,
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
            g.add(sn1, sn2);
            break;
        case 's':
            ifs >> sn1;
            g.startName(sn1);
            break;
        case 'e':
            ifs >> sn1;
            g.endName(sn1);
            break;
        case 'c':
            ifs >> sn1;
        }

        ifs >> stype;
    }
}
static void readObstacles(
    raven::graph::cGraph &g,
    std::ifstream &ifs)
{
    throw std::runtime_error(
        "Input file must be processed using the obstacle application");
}
static void readCycle(
    raven::graph::cGraph &g,
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
            g.add(sn1, sn2);
        }
        else if (stype[0] == 's')
        {
            ifs >> sn1;
            g.startName(sn1);
        }
        ifs >> stype;
    }
}
namespace raven
{
    namespace graph
    {
        graph_calc readfile(
            raven::graph::cGraph &g,
            std::vector<double>& edgeWeight,
            const std::string &fname)
        {
            raven::graph::graph_calc myCalcOption = raven::graph::graph_calc::none;
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
                myCalcOption = graph_calc::cost;
                readCostedLinks(g, edgeWeight, ifs);
            }

            else if (calc.find("cycle") != -1)
            {
                myCalcOption = graph_calc::cycle;
                readCycle(g, ifs);
            }

            else if (calc.find("astar") != -1)
            {
                myCalcOption = graph_calc::astar;
                std::string sn1, sn2;
                ifs >> sn1 >> sn2;

                while (ifs.good())
                {
                    g.add(sn1, sn2);
                    ifs >> sn1 >> sn2;
                }
            }
            else if (calc.find("probs") != -1)
            {
                myCalcOption = graph_calc::probs;
                readCostedLinks(g, edgeWeight, ifs);
            }
            else if (calc.find("tour") != -1)
            {
                myCalcOption = graph_calc::tour;
                readCostedLinks(g, edgeWeight, ifs);
            }
            else if (calc.find("obs") != -1)
            {
                myCalcOption = graph_calc::obs;
                readObstacles(g, ifs);
            }
            else if (calc.find("farm") != -1)
            {
                myCalcOption = graph_calc::obs;
                readObstacles(g, ifs);
            }
            else if (calc.find("spans") != -1)
            {
                myCalcOption = graph_calc::spans;
                readCostedLinks(g, edgeWeight, ifs);
            }
            else if (calc.find("sales") != -1)
            {
                myCalcOption = graph_calc::sales;
                readSales(g, edgeWeight, ifs);
            }
            else if (calc.find("cliques") != -1)
            {
                myCalcOption = graph_calc::cliques;
                readUncostedLinks(g, ifs);
            }
            else if (calc.find("multiflows") != -1)
            {
                myCalcOption = graph_calc::multiflows;
                readCostedLinks(g, edgeWeight, ifs);
            }
            else if (calc.find("flows") != -1)
            {
                myCalcOption = graph_calc::flows;
                readCostedLinks(g, edgeWeight, ifs);
            }
            else if (calc.find("allpaths") != -1)
            {
                myCalcOption = graph_calc::allpaths;
                readUncostedLinks(g, ifs);
            }
            else if (calc.find("alloc") != -1)
            {
                myCalcOption = graph_calc::alloc;
                readUncostedLinks(g, ifs);
            }
            else if (calc.find("euler") != -1)
            {
                myCalcOption = graph_calc::euler;
                readUncostedLinks(g, ifs);
            }
            else if (calc.find("cover") != -1)
            {
                myCalcOption = graph_calc::cover;
                readUncostedLinks(g, ifs);
            }
            else
                throw std::runtime_error(
                    "bad calculation type ");

            return myCalcOption;
        }
    }
}
