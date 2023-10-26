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
      myplLayout(wex::maker::make<wex::panel>(fm))
{
    myplText.move(0, 0, 800, 750);
    myplLayout.move(0, 100, 800, 750);

    myGraphData.option = raven::graph::graph_calc::none;

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
        myGraphData.fname = fb.open();

        {
            // raven::set::cRunWatch::Start();

            readfile(myGraphData);

            myplText.text("Calculating...");
            myplText.update();

            switch (myGraphData.option)
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

            case raven::graph::graph_calc::components:
                calcComponents();
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

            case raven::graph::graph_calc::cuts:
                calcCuts();
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
    auto result = path(myGraphData);

    if (!result.first.size())
    {
        myResultText = "No path found";
        return;
    }
    myResultText = "";
    for (int v : result.first)
        myResultText += myGraphData.g.userName(v) + " -> ";
    myResultText += " Cost = " + std::to_string(result.second);

    auto viz = pathViz(
        myGraphData.g,
        result.first,
        true);
    RunDOT(
        myGraphData.g,
        viz);
}

void cGUI::calcSpan()
{
    myResultGraph = spanningTree(myGraphData);

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
    auto vc = dfs_cycle_finder(myGraphData);

    myResultText = std::to_string(vc.size()) + " cycles found\n\n";
    for (int k = 0; k < vc.size(); k++)
    {
        for (auto &sv : myGraphData.g.userName(vc[k]))
            myResultText += sv + " ";

        myResultText += "\n";
    }
    myViewType = eView::route;

    RunDOT(
        myGraphData.g,
        pathViz(
            myGraphData.g,
            {},
            true));
}

void cGUI::calcAllPaths()
{
    auto vsp = raven::graph::allPaths(myGraphData);
    myResultText = "";
    for (auto &p : vsp)
    {
        for (int v : p.first)
        {
            myResultText += myGraphData.g.userName(v) + " -> ";
        }
        myResultText += " cost " + std::to_string(p.second) + "\n";
    }
    std::cout << myResultText;
}

void cGUI::calcTour()
{
    delete mypTourNodes;
    mypTourNodes = new raven::graph::cTourNodes();
    mypTourNodes->calculate(myGraphData);

    myResultText = "";
    for (int v : mypTourNodes->getTour())
    {
        myResultText += myGraphData.g.userName(v) + " -> ";
    }
    std::cout << myResultText << "\n";
}

void cGUI::calcSales()
{
    myResultText = "Sales calcultion NYI";

    if (myGraphData.g.rVertexAttr(0, 1) == "")
    {
        // link specification
    }
    else
    {
        // city location specification
        // link all cities with the square of the pythogorean distance between their locations
        for (int v1 = 0; v1 < myGraphData.g.vertexCount(); v1++)
        {
            double x1 = atof(myGraphData.g.rVertexAttr(v1, 0).c_str());
            double y1 = atof(myGraphData.g.rVertexAttr(v1, 1).c_str());
            for (int v2 = v1 + 1; v2 < myGraphData.g.vertexCount(); v2++)
            {
                double x2 = atof(myGraphData.g.rVertexAttr(v2, 0).c_str());
                double y2 = atof(myGraphData.g.rVertexAttr(v2, 1).c_str());
                double dx = x2 - x1;
                double dy = y2 - y1;
                double dsq = dx * dx + dy * dy;
                myGraphData.edgeWeight[myGraphData.g.add(v1, v2)] = dsq;
            }
        }
    }

    calcTour();

    RunDOT(
        myGraphData.g,
        pathViz(
            myGraphData.g,
            mypTourNodes->getTour(),
            true));
}

void cGUI::calcComponents()
{
    auto vclique = components(
        myGraphData.g);
    std::stringstream ss;
    std::string comp_clique = "component  ";
    for (auto &c : vclique)
    {
        ss << comp_clique;
        for (int n : c)
            ss << myGraphData.g.userName(n) << " ";
        ss << "\n";
    }
    myResultText = ss.str();
}
void cGUI::calcCliques()
{
    auto vclique = components(
        myGraphData.g,
        true);
    std::stringstream ss;
    std::string comp_clique = "clique ";
    for (auto &c : vclique)
    {
        ss << comp_clique;
        for (int n : c)
            ss << myGraphData.g.userName(n) << " ";
        ss << "\n";
    }
    myResultText = ss.str();
}

void cGUI::calcFlows()
{
    std::vector<int> vEdgeFlow;

    double flow = flows(
        myGraphData,
        vEdgeFlow);
    myResultText = "Total Flow = " + std::to_string(flow);
}
void cGUI::calcMultiFlows()
{
    double flow = multiflows(myGraphData);

    myResultText = "Total Flow = " + std::to_string(flow);
}

void cGUI::calcProbs()
{
    myResultText = "Probability " + std::to_string(probs(myGraphData));
}

void cGUI::calcAlloc()
{
    auto ret = alloc(myGraphData);

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
    auto ret = euler(myGraphData.g);

    std::stringstream ss;
    for (int vi : ret)
        ss << myGraphData.g.userName(vi) << " -> ";
    myResultText = ss.str();

    auto viz = pathViz(
        myGraphData.g,
        ret,
        true);
    RunDOT(
        myGraphData.g,
        viz);
}

void cGUI::calcCover()
{
    auto ret = vertexCover(myGraphData.g);

    std::stringstream ss;
    for (int vi : ret)
        ss << myGraphData.g.userName(vi) << ", ";
    myResultText = ss.str();
    myViewType = eView::route;
}

void cGUI::calcExplore()
{
    auto path = astar(
        myGraphData,
        [this](int v) -> double
        {
            return myGraphData.edgeWeight[v];
        },
        [this](int v) -> double
        {
            return 0;
        });
    myResultText = "";
    for (int v : path)
        myResultText += myGraphData.g.userName(v) + " -> ";
    RunDOT(
        myGraphData.g,
        pathViz(
            myGraphData.g,
            path,
            true));
}

void cGUI::calcCuts()
{
    raven::graph::cTarjan T;
    auto vAP = T.ArticulationPoints(myGraphData);
    if (!vAP.size())
    {
        myResultText = "The graph contains no cut vertices";
        return;
    }
    myResultText = "Cut vertices: ";
    for (auto &name : vAP)
        myResultText += name + ", ";
    return;
}
void cGUI::draw(PAINTSTRUCT &ps)
{
    fm.text("PathFinder " + myGraphData.fname);

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
    if (myGraphData.fname.empty())
        return;
    std::ifstream ifs(myGraphData.fname);
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
    switch (myGraphData.option)
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

    case raven::graph::graph_calc::cliques:
    {
        RunDOT(
            myGraphData.g,
            pathViz(
                myGraphData.g,
                {},
                false));

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
