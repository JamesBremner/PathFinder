#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "wex.h"
#include "window2file.h"
#include "GraphTheory.h"
#include "cPathFinderGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "PathFinder",
          {50, 50, 1000, 700}),
      myplText(wex::maker::make<wex::panel>(fm)),
      myplLayout(wex::maker::make<wex::panel>(fm)),
      myCalcOption(graph_calc::none)
{
    myplText.move(0, 0, 800, 750);
    myplLayout.move(0, 100, 800, 750);

    ConstructMenu();

    myplText.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            draw(ps);
        });

    myplLayout.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            drawLayout(ps);
        });

    show();
    run();
}

void cGUI::ConstructMenu()
{
    wex::menubar mbar(fm);
    wex::menu mfile(fm);
    mfile.append("Calculate",
                 [&](const std::string &title)
                 {
                     calculate();
                 });

    mbar.append("File", mfile);

    wex::menu vfile(fm);
    vfile.append(
        "Input",
        [&](const std::string &title)
        {
            myViewType = eView::input;
            fm.update();
        });
    vfile.append(
        "Spanning Tree",
        [&](const std::string &title)
        {
            myViewType = eView::span;
            fm.update();
        });
    vfile.append(
        "Route",
        [&](const std::string &title)
        {
            fm.update();
        });
    mbar.append("View", vfile);
}

void cGUI::calculate()
{
    wex::filebox fb(fm);

    try
    {
        myfname = fb.open();

        {
            // raven::set::cRunWatch::Start();
            myCalcOption = readfile(myGraph, myfname);

            myplText.text("Calculating...");
            myplText.update();

            switch (myCalcOption)
            {

            case graph_calc::cost:
                calcCost();
                break;

            case graph_calc::cycle:
                calcCycle();
                break;

            case graph_calc::tour:
                calcTour();
                break;

            case graph_calc::spans:
                calcSpan();
                break;

            case graph_calc::sales:
                calcSales();
                break;

            case graph_calc::cliques:
                calcCliques();
                break;

            case graph_calc::flows:
                calcFlows();
                break;

            case graph_calc::multiflows:
                calcMultiFlows();
                break;

            case graph_calc::allpaths:
                calcAllPaths();

            case graph_calc::none:
                break;
            }
        }
        myplText.text("");
        myplLayout.show(true);
        fm.update();
    }
    catch (std::runtime_error &e)
    {
        wex::msgbox m(e.what());
        return;
    }
}
void cGUI::calcCost()
{
    if (myStartName.empty() || myEndName.empty())
        throw std::runtime_error("No path endpoints");
    if( ! myStartName.size())
        throw std::runtime_error("No start");

    auto result = path(
        myGraph,
        myStartName.back(),
        myEndName);

    if (!result.first.size())
    {
        myResultText = "No path found";
        return;
    }
    myResultText = "";
    for (int v : result.first)
        myResultText += myGraph.userName(v) + " -> ";
    myResultText += " Cost = " + std::to_string(result.second);

    auto viz = pathViz(
        myGraph,
        result.first,
        true);
    RunDOT(
        myGraph,
        viz);
}

void cGUI::calcSpan()
{
    if (myStartName.empty())
        myStartName.push_back( myGraph.userName(0) );
    myResultGraph = spanningTree(myGraph, myStartName.back());
    myViewType = eView::span;
    auto viz = pathViz(
        myResultGraph,
        {},
        true);
    RunDOT(
        myResultGraph,
        viz);
}

void cGUI::calcCycle()
{
    auto vc = dfs_cycle_finder(myGraph);

    myResultText = std::to_string(vc.size()) + " cycles found\n\n";
    for (int k = 0; k < vc.size(); k++)
    {
        for (auto &sv : myGraph.userName(vc[k]))
            myResultText += sv + " ";

        myResultText += "\n";
    }
    myViewType = eView::route;
}

void cGUI::calcAllPaths()
{
    auto vc = allPaths(
        myGraph,
        myStartName.back(),
        myEndName);
    myResultText = std::to_string(vc.size()) + " paths found\n\n";
    for (int k = 0; k < vc.size(); k++)
    {
        for (auto &sv : myGraph.userName(vc[k]))
            myResultText += sv + " ";

        myResultText += "\n";
    }
    myViewType = eView::route;
}


void cGUI::calcTour()
{
    delete mypTourNodes;
    mypTourNodes = new raven::graph::cTourNodes();
    mypTourNodes->calculate(myGraph);

    myResultText = "";
    for (int v : mypTourNodes->getTour())
    {
        myResultText += myGraph.userName(v) + " -> ";
    }
}

void cGUI::calcSales()
{
    myResultText = "Sales calcultion NYI";

    if (myGraph.rVertexAttr(0, 1) == "")
    {
        // link specification
    }
    else
    {
        // city location specification
        // link all cities with the square of the pythogorean distance between their locations
        for (int v1 = 0; v1 < myGraph.vertexCount(); v1++)
        {
            double x1 = atof(myGraph.rVertexAttr(v1, 0).c_str());
            double y1 = atof(myGraph.rVertexAttr(v1, 1).c_str());
            for (int v2 = v1 + 1; v2 < myGraph.vertexCount(); v2++)
            {
                double x2 = atof(myGraph.rVertexAttr(v2, 0).c_str());
                double y2 = atof(myGraph.rVertexAttr(v2, 1).c_str());
                double dx = x2 - x1;
                double dy = y2 - y1;
                std::string dsq = std::to_string(dx * dx + dy * dy);
                myGraph.wEdgeAttr(myGraph.findorAdd(v1, v2), {dsq});
            }
        }
    }

    calcTour();

    RunDOT(
        myGraph,
        pathViz(
            myGraph,
            mypTourNodes->getTour(),
            true));
}

void cGUI::calcCliques()
{
    cliques(
        myGraph,
        myResultText);
}

void cGUI::calcFlows()
{
    double flow = flows(
        myGraph,
        myGraph.find(myStartName.back()),
        myGraph.find(myEndName));
    myResultText = "Total Flow = " + std::to_string(flow);
}
void cGUI::calcMultiFlows()
{
    std::vector<int> vstart;
    for( auto& n : myStartName )
        vstart.push_back( myGraph.find(n) );
    double flow = multiflows(
        myGraph,
        vstart,
        myGraph.find( myEndName)    );
    myResultText = "Total Flow = " + std::to_string(flow);
}
void cGUI::draw(PAINTSTRUCT &ps)
{
    fm.text("PathFinder " + myfname);

    wex::shapes S(ps);

    switch (myViewType)
    {

    case eView::route:
        S.text(myResultText, {20, 20, 500, 500});
        break;

    case eView::input:
        drawInput(S);
        break;

    case eView::span:
        drawSpan(S);
        break;
    }
}
void cGUI::drawInput(wex::shapes &S)
{
    if (myfname.empty())
        return;
    std::ifstream ifs(myfname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open input file");
    std::string line;
    std::stringstream ss;
    while (getline(ifs, line))
    {
        ss << line << "\n";
    }
    S.text(ss.str(), {5, 5, 1000, 1000});
}

void cGUI::drawSpan(wex::shapes &S)
{
    S.color(0x000000);
    S.penThick(2);
    std::stringstream ss;
    for (int v1 = 0; v1 < myResultGraph.vertexCount(); v1++)
    {
        for (int v2 : myResultGraph.adjacentOut(v1))
        {
            // check not reverse link in undirected graph
            if (!myResultGraph.isDirected())
                if (v1 > v2)
                    continue;

            ss << myResultGraph.userName(v1) << " - "
               << myResultGraph.userName(v2) << ", ";
        }
    }
    S.text(ss.str(), {20, 20, 1000, 500});
}

void cGUI::drawLayout(PAINTSTRUCT &ps)
{
    switch (myCalcOption)
    {

    case graph_calc::cost:
    case graph_calc::spans:
    case graph_calc::sales:
    {
        // fill graph panel with image produced by graphviz
        myplLayout.show(true);
        wex::window2file w2f;
        auto path = std::filesystem::temp_directory_path();
        auto sample = path / "sample.png";
        w2f.draw(myplLayout, sample.string());
    }
    break;

    default:
        myplLayout.show(false);
        break;
    }
}
