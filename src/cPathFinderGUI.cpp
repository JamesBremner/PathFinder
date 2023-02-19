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

            myplText.text("Caclulating...");
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

                delete mypTourNodes;
                mypTourNodes = new raven::cTourNodes(myGraph);
                mypTourNodes->calculate();

                myResultText = "";
                for (int v : mypTourNodes->getTour())
                {
                    myResultText += myGraph.userName(v) + " -> ";
                }
                break;

            case graph_calc::spans:
                calcSpan();
                break;

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
    if( myStartName.empty() || myEndName.empty() )
        throw std::runtime_error("No path endpoints");
        
    auto result = path(
        myGraph,
        myStartName,
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
    myResultGraph = spanningTree(myGraph, myGraph.userName(0));
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
    auto vc = dfs_cycle_finder(myGraph, myGraph.userName(0));
    myResultText = std::to_string(vc.size()) + " cycles found\n\n";
    for (int k = 0; k < vc.size(); k++)
    {
        for (auto &sv : myGraph.userName(vc[k]))
            myResultText += sv + " ";

        myResultText += "\n";
    }
    myViewType = eView::route;
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
