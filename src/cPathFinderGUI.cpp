#include <string>
#include <fstream>
#include "wex.h"
#include "GraphTheory.h"
#include "cPathFinderGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "Graphify",
          {50, 50, 1000, 700})
{
    ConstructMenu();

    fm.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            draw(ps);
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
            fm.update();
        });
    vfile.append("Spanning Tree",
                 [&](const std::string &title)
                 {
                     fm.update();
                 });
    vfile.append("Route",
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
            //raven::set::cRunWatch::Start();

            switch (readfile(myGraph, myfname))
            {
            case graph_calc::cost:
            {
                myResultText = "";
                for (int v : path(
                        myGraph,
                         myStartName,
                         myEndName))
                {
                    myResultText += myGraph.userName( v ) + " ";
                }
            }
            break;

            case graph_calc::cycle:
            {
                // auto vc = myGraph.dfs_cycle_finder(myGraph.userName(0));
                // myResultText = std::to_string(vc.size()) + " cycles found";
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

void cGUI::draw(PAINTSTRUCT &ps)
{
    // if (myObstacle.view() == -999)
    //     return;

    fm.text("Graphify " + myfname);

    wex::shapes S(ps);

    S.text(myResultText, {20, 20});
}
