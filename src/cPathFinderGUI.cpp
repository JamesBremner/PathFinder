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
     graphPanel( wex::maker::make<wex::panel>(fm)),
     myCalcOption( graph_calc::none )
{

    graphPanel.move(0, 50, 800, 750);

    ConstructMenu();

    fm.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            draw(ps);
        });

    graphPanel.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            if( myCalcOption == graph_calc::none )
                return;
            // fill graph panel with image produced by graphviz
            wex::window2file w2f;
            auto path = std::filesystem::temp_directory_path();
            auto sample = path / "sample.png";
            w2f.draw(graphPanel, sample.string());
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

            case graph_calc::none:
                break;
            }
        }
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
    auto result = path(
        myGraph,
        myStartName,
        myEndName);

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
    graphPanel.update();
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
    if (myCalcOption != graph_calc::tour)
        return;
    S.color(0x0000FF);
    S.penThick(2);
    std::stringstream ss;
    for (auto &pl : mypTourNodes->spanTree_get())
    {
        ss << myGraph.userName(std::get<0>(pl))
           << " - " << myGraph.userName(std::get<1>(pl))
           << "\n";
        // int w, h, w2, h2;
        // grid->coords(
        //     w, h, std::get<0>(pl));
        // grid->coords(
        //     w2, h2, std::get<1>(pl));
        // S.line({scale * w, scale * h, scale * w2, scale * h2});
    }
    S.text(ss.str(), {20, 20, 1000, 500});
}
