#include <string>
#include <fstream>
#include "wex.h"
// #include <autocell.h>
// #include "cxy.h"
// #include "cObstacle.h"
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

    int scale = 40;

    wex::shapes S(ps);
    int W, H;
    myObstacle.size(W, H);

    auto grid = myObstacle.grid();
    S.color(0x000000);

    std::stringstream sspath;
    int pathCount = 0;
    switch (myViewType)
    {
    case eView::input:
        drawInput(S);
        break;

    case eView::route:
        drawObstacles(S, scale);
        drawTour(S, scale);
        break;

    case eView::routelist:
        drawRouteList(S);
        break;

    case eView::span:
        drawObstacles(S, scale);
        drawSpanningTree(S, scale);
        break;

    case eView::calculating:
        S.text("Calculating...", {10, 10});
        break;
    }
}
void cGUI::drawSpanningTree(
    wex::shapes &S,
    int scale )
{
    auto* grid = myObstacle.grid();
    S.color(0x0000FF);
    S.penThick(2);
    for (auto &pl : myObstacle.spanningTree_get())
    {
        int w, h, w2, h2;
        grid->coords(
            w, h, grid->cell(pl.first));
        grid->coords(
            w2, h2, grid->cell(pl.second));
        S.line({scale * w, scale * h, scale * w2, scale * h2});
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

    S.text(ss.str(), {5, 5, 1000, 1000});
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
    S.textHeight(10);
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
    S.textHeight(20);
}

void cGUI::drawTour(wex::shapes &S, int scale)
{
    if (!myObstacle.tour().size())
    {
        std::cout << "cGUI::drawTour no tour found\n";
        return;
    }

    int W, H;
    myObstacle.size(W, H);
    std::stringstream ss;
    auto prev = myObstacle.tour()[0];
    int k = 0;
    for (auto &loc : myObstacle.tour())
    {
        S.line({scale * std::get<1>(prev), scale * std::get<2>(prev),
                scale * std::get<1>(loc), scale * std::get<2>(loc)});
        ss << std::get<0>(loc) << "->";
        if (k++ > 12)
        {
            ss << "\n";
            k = 0;
        }
        prev = loc;
    }
    ss << "\n\rUnvisited " << myObstacle.unvisitedCount()
       << " Revisited " << myObstacle.revisitedCount();

    S.text(ss.str(), {scale, H * scale, 1000, 1000});
}
