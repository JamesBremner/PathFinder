#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include "cGraph.h"
#include "GraphTheory.h"
#include "cGrid2d.h"

static void readSales(
    raven::graph::sGraphData &graphData,
    std::ifstream &ifs)
{
    graphData.g.clear();

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
            graphData.g.wVertexAttr(graphData.g.add(name), {x, y});
            break;

        case 'l':
            if (inputType == eInput::city)
                throw std::runtime_error(
                    "Cannot mix cities and links");
            inputType = eInput::link;
            ifs >> sn1 >> sn2 >> scost;
            graphData.g.add(sn1, sn2);

            /* Store the edge weight
               An undirected graph is modelled with 2 edges,
               backwards and forwards between the vertices */
            double c = atof(scost.c_str());
            graphData.edgeWeight.push_back(c);
            graphData.edgeWeight.push_back(c);

            break;
        }

        ifs >> stype;
    }
}

static void readCostedLinks(
    raven::graph::sGraphData &graphData,
    std::ifstream &ifs)
{
    graphData.g.clear();
    graphData.multiStart.clear();

    std::string stype, sn1, sn2, scost, directed, same;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'g':
            if (graphData.g.vertexCount())
                throw std::runtime_error(
                    "g ( graph mode ) must be second line");
            ifs >> directed >> same;
            if (directed == "1")
                graphData.g.directed();
            break;
        case 'l':
            ifs >> sn1 >> sn2 >> scost;
            graphData.g.add(sn1, sn2);
            graphData.edgeWeight.push_back(atof(scost.c_str()));
            if ((!graphData.g.isDirected()) && same == "1")
            {
                graphData.edgeWeight.push_back(atof(scost.c_str()));
            }
            break;
        case 's':
            ifs >> graphData.startName;
            graphData.multiStart.push_back(graphData.startName);
            break;
        case 'e':
            ifs >> graphData.endName;
            break;
        case 'c':
            ifs >> sn1;
        }

        ifs >> stype;
    }
}
static void readUncostedLinks(
    raven::graph::sGraphData &gd,
    std::ifstream &ifs)
{
    gd.g.clear();

    std::string stype, sn1, sn2;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'g':
            if (gd.g.vertexCount())
                throw std::runtime_error(
                    "g ( graph mode ) must be second line");
            gd.g.directed();
            break;
        case 'l':
            ifs >> sn1 >> sn2;
            gd.g.add(sn1, sn2);
            break;
        case 's':
            ifs >> gd.startName;
            break;
        case 'e':
            ifs >> gd.endName;
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
    raven::graph::sGraphData &graphData,
    std::ifstream &ifs)
{
    graphData.g.clear();
    graphData.g.directed();
    std::string stype, sn1, sn2;
    ifs >> stype;

    while (ifs.good())
    {
        if (stype[0] == 'l')
        {
            ifs >> sn1 >> sn2;
            graphData.g.add(sn1, sn2);
        }
        else if (stype[0] == 's')
        {
            ifs >> graphData.startName;
        }
        ifs >> stype;
    }
}

static void readExplore(
    raven::graph::sGraphData &graphData,
    std::ifstream &ifs)
{
    graphData.g.clear();
    graphData.g.directed();

    cGrid2D grid;
    std::vector<int> vBlockCells;

    std::string stype;
    int dim, sc, sr;
    ifs >> stype;
    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'd':
        {
            ifs >> dim;
            grid.setDim(dim, dim);
            graphData.edgeWeight.resize(8 * dim * dim, 1000);
            break;
        }
        case 'b':
        {
            int col, row;
            ifs >> col >> row;
            vBlockCells.push_back(grid.index(col, row));
            break;
        }
        case 's':
            ifs >> sc >> sr;
            graphData.startName = "c" + std::to_string(sc) + "r" + std::to_string(sr);
            break;
        case 'e':
            ifs >> sc >> sr;
            graphData.endName = "c" + std::to_string(sc) + "r" + std::to_string(sr);
            break;
        }
        ifs >> stype;
    }
    for (
        int cell = 0;
        cell < dim * dim;
        cell++)
    {
        int col, row;
        grid.coords(col, row, cell);

        double w = 1;
        if (std::find(
                vBlockCells.begin(),
                vBlockCells.end(),
                cell) != vBlockCells.end())
            w = 1000;

        auto srcname = grid.name(cell);

        for (int r = row - 1; r < row + 2; r++)
        {
            for (int c = col - 1; c < col + 2; c++)
            {
                if (c == col && r == row)
                    continue;
                int i = grid.index(c, r);
                if (i >= 0)
                    graphData.edgeWeight[graphData.g.add(srcname, grid.name(i))] = w;
            }
        }
    }
}

static std::vector<std::string>
tokenize(
    const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

static void readHills(
    raven::graph::sGraphData &graphData,
    std::ifstream &ifs)
{
    graphData.g.clear();
    graphData.g.directed();

    cGrid2D grid;
    std::vector<double> vHeight;

    std::string line;
    int colcount = -1;
    int rowCount = 0;
    int startRow, startCol, endRow, endCol;

    while (getline(ifs, line))
    {
        auto vt = tokenize(line);
        switch (line[0])
        {
        case 'o':
        {
            bool firstcol = true;
            for (auto &e : vt)
            {
                if (firstcol)
                    firstcol = false;
                else
                    vHeight.push_back(atof(e.c_str()));
            }
            if (colcount == -1)
                colcount = vt.size() - 1;
            else if (colcount != vt.size() - 1)
                throw std::runtime_error(
                    "readHills: variable column count");
            rowCount++;
        }
        break;

        case 's':
            if (vt.size() < 3)
                throw std::runtime_error(
                    "readHills: bad start line");
            startCol = atoi(vt[1].c_str()) - 1;
            startRow = atoi(vt[2].c_str()) - 1;
            break;

        case 'e':
            if (vt.size() < 3)
                throw std::runtime_error(
                    "readHills: bad end line");
            endCol = atoi(vt[1].c_str()) - 1;
            endRow = atoi(vt[2].c_str()) - 1;
            break;
        }
    }
    grid.setDim(colcount, rowCount);
    grid.addOrthoEdges();
    grid.addDiagEdges();
    for (auto &p : grid.getEdgesVertexIndex())
    {
        graphData.g.add(p.first, p.second);
        double delta = fabs(vHeight[p.first] - vHeight[p.second]);
        graphData.edgeWeight.push_back(1 + delta * delta);
    }
    graphData.startName = graphData.g.userName(grid.index(startCol, startRow));
    graphData.endName = graphData.g.userName(grid.index(endCol, endRow));
}

namespace raven
{
    namespace graph
    {
        void readfile(sGraphData &graphData)
        {
            graphData.option = raven::graph::graph_calc::none;
            std::ifstream ifs(graphData.fname);
            if (!ifs.is_open())
                throw std::runtime_error(
                    "Cannot open input file");

            std::string form, calc;
            ifs >> form >> calc;
            if (form != "format")
                throw std::runtime_error(
                    "Bad input format");

            else if (calc.find("allpathscosted") != -1)
            {
                graphData.option = graph_calc::allpaths;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("allpaths") != -1)
            {
                graphData.option = graph_calc::allpaths;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("cost") != -1)
            {
                graphData.option = graph_calc::cost;
                readCostedLinks(graphData, ifs);
            }

            else if (calc.find("cycle") != -1)
            {
                graphData.option = graph_calc::cycle;
                readCycle(graphData, ifs);
            }

            else if (calc.find("astar") != -1)
            {
                graphData.option = graph_calc::astar;
                std::string sn1, sn2;
                ifs >> sn1 >> sn2;

                while (ifs.good())
                {
                    graphData.g.add(sn1, sn2);
                    ifs >> sn1 >> sn2;
                }
            }
            else if (calc.find("probs") != -1)
            {
                graphData.option = graph_calc::probs;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("tour") != -1)
            {
                graphData.option = graph_calc::tour;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("obs") != -1)
            {
                graphData.option = graph_calc::obs;
                readObstacles(graphData.g, ifs);
            }
            else if (calc.find("farm") != -1)
            {
                graphData.option = graph_calc::obs;
                readObstacles(graphData.g, ifs);
            }
            else if (calc.find("spans") != -1)
            {
                graphData.option = graph_calc::spans;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("sales") != -1)
            {
                graphData.option = graph_calc::sales;
                readSales(graphData, ifs);
            }
            else if (calc.find("cliques") != -1)
            {
                graphData.option = graph_calc::cliques;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("components") != -1)
            {
                graphData.option = graph_calc::components;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("multiflows") != -1)
            {
                graphData.option = graph_calc::multiflows;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("flows") != -1)
            {
                graphData.option = graph_calc::flows;
                readCostedLinks(graphData, ifs);
            }
            else if (calc.find("alloc") != -1)
            {
                graphData.option = graph_calc::alloc;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("euler") != -1)
            {
                graphData.option = graph_calc::euler;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("cover") != -1)
            {
                graphData.option = graph_calc::cover;
                readUncostedLinks(graphData, ifs);
            }
            else if (calc.find("explore") != -1)
            {
                graphData.option = graph_calc::explore;
                readExplore(graphData, ifs);
            }
            else if (calc.find("hills") != -1)
            {
                graphData.option = graph_calc::cost;
                readHills(graphData, ifs);
            }
            else if (calc.find("cuts") != -1)
            {
                graphData.option = graph_calc::cuts;
                readUncostedLinks(graphData, ifs);
            }
            else
                throw std::runtime_error(
                    "bad calculation type ");
        }
    }
}
