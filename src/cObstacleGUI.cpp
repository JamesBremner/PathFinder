#include <string>
#include <fstream>
#include "wex.h"
#include <autocell.h>
#include "cxy.h"
#include "cObstacle.h"
#include "cObstacleGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "Obstacles",
          {50, 50, 1000, 700}),
      myViewType(eView::route)
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
                     wex::filebox fb(fm);

                     try
                     {
                         myfname = fb.open();
                         read(myObstacle, myfname);

                        myViewType = eView::calculating;
                        fm.update();
                        myViewType = eView::route;
                         
                         myObstacle.unobstructedPoints();

                         myObstacle.connect();
                         myObstacle.tourNodes();

                         fm.update();
                     }
                     catch (std::runtime_error &e)
                     {
                         wex::msgbox m(e.what());
                         return;
                     }
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
        "Route plot",
        [&](const std::string &title)
        {
            myViewType = eView::route;
            fm.update();
        });
    vfile.append(
        "Route list",
        [&](const std::string &title)
        {
            myViewType = eView::routelist;
            fm.update();
        });
    mbar.append("View", vfile);
}

void cGUI::draw(PAINTSTRUCT &ps)
{
    if (myObstacle.view() == -999)
        return;

    fm.text("Obstacles " + myfname);

    int scale = 10;

    wex::shapes S(ps);
    int W, H;
    myObstacle.size(W, H);

    auto grid = myObstacle.grid();
    S.color(0x000000);
    // for (auto &l : myObstacle.links())
    // {
    //     int w, h, w2, h2;
    //     grid->coords(
    //         w, h, std::get<0>(l));
    //     // std::cout << w << " " << h << " -> ";
    //     grid->coords(
    //         w2, h2, std::get<1>(l));
    //     // std::cout << w2 << " " << h2 << "\n";
    //     S.line({20 * w, 20 * h, 20 * w2, 20 * h2});
    // }

    std::stringstream sspath;
    int pathCount = 0;
    switch (myViewType)
    {
    case eView::input:
        drawInput(S);
        break;

    case eView::route:

        drawObstacles(S, scale);

        S.color(0xFF0000);
        S.penThick(1);
        // sspath << std::get<0>(myObstacle.path()[0])->ID();
        // for (auto &pl : myObstacle.path())
        // {
        //     auto n1 = std::get<0>(pl);
        //     auto n2 = std::get<1>(pl);
        //     int w, h, w2, h2;
        //     grid->coords(
        //         w, h, n1);
        //     grid->coords(
        //         w2, h2, n2);
        //     // S.line({scale * w, scale * h, scale * w2, scale * h2});

        //     sspath << " -> " << n2->ID();
        //     pathCount++;
        //     if (pathCount > 15)
        //     {
        //         sspath << "\r\n";
        //         pathCount = 0;
        //     }
        // }

        drawTour(S, scale);

        //S.text(sspath.str(), {scale, H * scale, 1000, 1000});
        // S.text(
        //     "Nodes revisited " +
        //     std::to_string(myObstacle.NodesRevisited().size()),
        //     {10,H*scale+30});
        std::cout << "Nodes revisited " << myObstacle.NodesRevisited().size() << " : ";
        for (auto n : myObstacle.NodesRevisited())
            std::cout << n->ID() << " ";

        break;

    case eView::routelist:
        drawRouteList(S);
        break;

    case eView::span:
        // S.color(0x0000FF);
        // S.penThick(2);
        // for (auto &pl : myObstacle.spanningTree_get())
        // {
        //     int w, h, w2, h2;
        //     grid->coords(
        //         w, h, std::get<0>(pl));
        //     grid->coords(
        //         w2, h2, std::get<1>(pl));
        //     S.line({scale * w, scale * h, scale * w2, scale * h2});
        // }
        break;

        case eView::calculating:
            S.text("Calculating...",{10,10});
            break;
    }
}
void cGUI::drawRouteList(wex::shapes &S)
{
    std::stringstream ss;
    for (auto &loc : myObstacle.tour())
    {
        ss << std::get<0>(loc) << "->";
    }
    ss << "\n\rUnvisited " << myObstacle.unvisitedCount() 
        << " Revisited " << myObstacle.revisitedCount();

    S.text(ss.str(), {5,5, 1000, 1000});


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
void cGUI::drawObstacles(
    wex::shapes &S,
    int scale)
{
    int W, H;
    myObstacle.size(W, H);
    S.color(0x0000FF);
    for (int h = 0; h < H; h++)
        for (int w = 0; w < W; w++)
        {
            auto s = myObstacle.draw(w, h);
            if (s.empty())
                continue;
            if (s == "X")
            {
                S.color(0x0000FF);
                S.text(s, {w * scale, h * scale});
                continue;
            }
            S.color(0x000000);
            S.text(s, {w * scale, h * scale});
        }
}

void cGUI::drawTour(wex::shapes &S, int scale)
{

    int W, H;
    myObstacle.size(W, H);
    std::stringstream ss;
    auto prev = myObstacle.tour()[0];
    for (auto &loc : myObstacle.tour())
    {
        S.line({scale * std::get<1>(prev), scale * std::get<2>(prev),
                scale * std::get<1>(loc), scale * std::get<2>(loc)});
        ss << std::get<0>(loc) << "->";
        prev = loc;
    }
    ss << "\n\rUnvisited " << myObstacle.unvisitedCount() 
        << " Revisited " << myObstacle.revisitedCount();

    S.text(ss.str(), {scale, H * scale, 1000, 1000});
}
