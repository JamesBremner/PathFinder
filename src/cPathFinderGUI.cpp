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
      myCalcOption(raven::graph::graph_calc::none)
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
            myplText.show();
            myplLayout.show(false);
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
            myplText.show();
            myplLayout.show(false);
            myViewType = eView::route;
            fm.update();
        });
    vfile.append(
        "Layout",
        [&](const std::string &title)
        {
            myplText.show(false);
            myplLayout.update();
            myplLayout.show();
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

            myCalcOption = readfile(
                myGraph,
                myEdgeWeight,
                myfname);

            myplText.text("Calculating...");
            myplText.update();

            switch (myCalcOption)
            {

            case raven::graph::graph_calc::cost:
                calcCost();
                break;

            case raven::graph::graph_calc::cycle:
                calcCycle();
                break;

            case raven::graph::graph_calc::tour:
                calcTour();
                break;

            case raven::graph::graph_calc::spans:
                calcSpan();
                break;

            case raven::graph::graph_calc::sales:
                calcSales();
                break;

            case raven::graph::graph_calc::cliques:
                calcCliques();
                break;

            case raven::graph::graph_calc::flows:
                calcFlows();
                break;

            case raven::graph::graph_calc::multiflows:
                calcMultiFlows();
                break;

            case raven::graph::graph_calc::allpaths:
                calcAllPaths();
                break;

            case raven::graph::graph_calc::probs:
                calcProbs();
                break;

            case raven::graph::graph_calc::alloc:
                calcAlloc();
                break;

            case raven::graph::graph_calc::euler:
                calcEuler();
                break;

            case raven::graph::graph_calc::cover:
                calcCover();
                break;

            case raven::graph::graph_calc::explore:
                calcExplore();
                break;

            case raven::graph::graph_calc::none:
                break;
            }
        }
        myplText.text("");
        myplText.show();
        myplLayout.show(false);
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
    myStartName.clear();
    myStartName.push_back(myGraph.startName());
    myEndName = myGraph.endName();
    if (myStartName.empty() || myEndName.empty())
        throw std::runtime_error("No path endpoints");
    if (!myStartName.size())
        throw std::runtime_error("No start");

    auto result = path(
        myGraph,
        myEdgeWeight,
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
    myStartName.clear();
    if (myGraph.startName().empty())
        myStartName.push_back(myGraph.userName(0));
    else
        myStartName.push_back(myGraph.startName());
    myResultGraph = spanningTree(
        myGraph,
        myEdgeWeight,
        myStartName.back());
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
    int startIndex = -1;
    if (myStartName.size())
        startIndex = myGraph.find(myStartName[0]);
    auto vc = dfs_cycle_finder(
        myGraph,
        startIndex);

    myResultText = std::to_string(vc.size()) + " cycles found\n\n";
    for (int k = 0; k < vc.size(); k++)
    {
        for (auto &sv : myGraph.userName(vc[k]))
            myResultText += sv + " ";

        myResultText += "\n";
    }
    myViewType = eView::route;

    RunDOT(
        myGraph,
        pathViz(
            myGraph,
            {},
            true));
}

void cGUI::calcAllPaths()
{
    auto vc = dfs_allpaths(
        myGraph,
        myGraph.find(myGraph.startName()),
        myGraph.find(myGraph.endName()));
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
    mypTourNodes->calculate(myGraph, myEdgeWeight);

    myResultText = "";
    for (int v : mypTourNodes->getTour())
    {
        myResultText += myGraph.userName(v) + " -> ";
    }
    std::cout << myResultText << "\n";
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
                double dsq = dx * dx + dy * dy;
                myEdgeWeight[myGraph.add(v1, v2)] = dsq;
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
    std::vector<int> vEdgeFlow;
    myStartName.clear();
    myStartName.push_back(myGraph.startName());
    myEndName = myGraph.endName();
    double flow = flows(
        myGraph,
        myEdgeWeight,
        myGraph.find(myStartName.back()),
        myGraph.find(myEndName),
        vEdgeFlow);
    myResultText = "Total Flow = " + std::to_string(flow);
}
void cGUI::calcMultiFlows()
{
    std::vector<int> vstart;
    for (auto &n : myStartName)
        vstart.push_back(myGraph.find(n));
    double flow = multiflows(
        myGraph,
        myEdgeWeight,
        vstart,
        myGraph.find(myEndName));
    myResultText = "Total Flow = " + std::to_string(flow);
}

void cGUI::calcProbs()
{
    double p = probs(
        myGraph,
        myEdgeWeight,
        myGraph.find(myGraph.endName()));
    myResultText = "Probability " + std::to_string(p);
}

void cGUI::calcAlloc()
{
    auto ret = alloc(myGraph);

    std::stringstream ss;
    ss << "Agent                Task\n\n";
    for (int k = 0; k < ret.size(); k += 2)
    {
        ss << std::setw(20) << std::left << ret[k] << ret[k + 1] << "\n";
    }
    myResultText = ss.str();
}

void cGUI::calcEuler()
{
    auto ret = euler(myGraph);

    std::stringstream ss;
    for (int vi : ret)
        ss << myGraph.userName(vi) << " -> ";
    myResultText = ss.str();

    auto viz = pathViz(
        myGraph,
        ret,
        true);
    RunDOT(
        myGraph,
        viz);
}

void cGUI::calcCover()
{
    auto ret = vertexCover(myGraph);

    std::stringstream ss;
    for (int vi : ret)
        ss << myGraph.userName(vi) << ", ";
    myResultText = ss.str();
    myViewType = eView::route;
}

void cGUI::calcExplore()
{
    auto path = astar(
        myGraph,
        [this](int v) -> double
        {
            return myEdgeWeight[v];
        },
        myGraph.find(myGraph.startName()),
        myGraph.find(myGraph.endName()),
        [this](int v) -> double
        {
            return 0;
        });
    myResultText = "";
    for (int v : path)
        myResultText += myGraph.userName(v) + " -> ";
    RunDOT(
        myGraph,
        pathViz(
            myGraph,
            path,
            true));
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

    case raven::graph::graph_calc::cost:
    case raven::graph::graph_calc::spans:
    case raven::graph::graph_calc::sales:
    case raven::graph::graph_calc::euler:
    case raven::graph::graph_calc::cycle:
    case raven::graph::graph_calc::explore:
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
