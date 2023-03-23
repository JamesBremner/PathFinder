#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "cxy.h"
#include <autocell.h>
#include "cObstacle.h"
#include "cGraph.h"
#include "GraphTheory.h"

void read(
    cObstacle &obs,
    const std::string &fname)
{
    obs.clear();
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open input file");
    std::string form, calc;
    ifs >> form >> calc;
    if (form != "format")
        throw std::runtime_error(
            "Bad input format");

    if (calc.find("obs") != -1)
    {
        int nx, ny, view;
        ifs >> nx >> ny >> view;
        obs.grid(nx, ny);
        obs.view(view);
        while (ifs.good())
        {
            // insert obstacle
            int ox, oy;
            ifs >> ox >> oy;
            obs.obstacle(ox, oy);
        }
    }

    else if (calc.find("farm") != -1)
    {
        int nx, ny;
        std::string sid;
        obs.view(-1);
        ifs >> sid;
        if (sid[0] == 'p')
            obs.poly();
        else
        {
            ifs >> nx >> ny;
            obs.grid(nx, ny);
            return;
        }
        ifs >> nx >> ny;
        obs.polyAdd(cxy(nx, ny));
        while (ifs.good())
        {
            ifs >> sid >> nx >> ny;
            if (sid[0] == 'p')
                obs.polyAdd(cxy(nx, ny));
        }
    }
    else
        throw std::runtime_error(
            "bad calculation type ");
}

void cObstacle::obstacle(int x, int y)
{
    if (0 > x || x > nx - 1 || 0 > y || y >> ny - 1)
        throw std::runtime_error("Bad location");
    A->cell(x, y)->myType = 1;
}

void cObstacle::grid(int x, int y)
{
    nx = x;
    ny = y;
    A = new cell::cAutomaton<cOCell>(nx, ny);
}

bool cObstacle::isBlocked(int x1, int y1, int x2, int y2)
{
    int W, H;
    A->size(W, H);
    for (int w = 0; w < W; w++)
        for (int h = 0; h < H; h++)
        {
            // check for obstacle
            if (A->cell(w, h)->myType != 1)
                continue;

            // check for obstacle close to connection
            int ow, oh;
            A->coords(ow, oh, A->cell(w, h));

            cxy obstacle(ow, oh);
            cxy line1(x1, y1);
            cxy line2(x2, y2);
            if (obstacle.dis2toline(line1, line2) < 2)
                return true;
        }
    // no obstacle found close to connection
    return false;
}

void cObstacle::connect()
{
    // loop over node pairs
    for (auto n1 : vN)
        for (auto n2 : vN)
        {
            // do not self connect
            if (n1 == n2)
                continue;

            // do not connect nodes that are not neighbours
            int w1, h1, w2, h2;
            A->coords(w1, h1, n1);
            A->coords(w2, h2, n2);
            int dx = w1 - w2;
            int dy = h1 - h2;
            int d2 = dx * dx + dy * dy;
            if (d2 > 50)
                continue;

            // check for blocked
            if (isBlocked(w1, h1, w2, h2))
                continue;

            // OK to connect

            auto sn1 = std::to_string(n1->ID());
            auto sn2 = std::to_string(n2->ID());
            mygraphdata.wEdgeAttr(
                mygraphdata.add(sn1, sn2),
                {std::to_string(d2)});
            mygraphdata.wEdgeAttr(
                mygraphdata.add(sn2, sn1),
                {std::to_string(d2)});
            mygraphdata.wVertexAttr(
                mygraphdata.find(sn1),
                {std::to_string(w1), std::to_string(h1)});
            mygraphdata.wVertexAttr(
                mygraphdata.find(sn2),
                    {std::to_string(w2),std::to_string(h2)});
        }
}

void cObstacle::tourNodes()
{
    myRouteCalculator.calculate(mygraphdata);
    std::cout << "unvisited " << myRouteCalculator.unvisitedCount()
              << ", revisited " << myRouteCalculator.revisitedCount()
              << ", nodes " << mygraphdata.vertexCount()
              << "\n";

    myTour.clear();
    std::tuple<std::string,int,int> loc;
    for( int n : myRouteCalculator.getTour() ) {
        std::get<0>(loc) = mygraphdata.userName(n);
        A->coords(
            std::get<1>(loc),std::get<2>(loc),
            A->cell(atoi(std::get<0>(loc).c_str())));

        myTour.push_back( loc );
    }
}

std::string cObstacle::draw(int w, int h) const
{
    switch (A->cell(w, h)->myType)
    {
    case 0:
    default:
        return "";
    case 1:
        return "X";
    case 2:
        return std::to_string(A->cell(w, h)->ID());
    }
}



void cObstacle::clear()
{
    myView = -999;
    myfrect = true;
    vN.clear(); ///< nodes to be included in path
    myPolygon.clear();

}


void cObstacle::unobstructedPoints()
{
    int W, H;

    if (myfrect)
        A->size(W, H);
    else
    {
        cxy wh = cxy::enclosingWidthHeight(myPolygon);
        W = wh.x;
        H = wh.y;
        grid(W, H);
    }

    int V;
    if (myView > 0)
        V = myView;
    else
        V = 2;

    for (int h = V; h < H - V + 1; h += 2 * V + 1)
        for (int w = V; w <= W - V + 1; w += 2 * V + 1)
        {
            if (!myfrect)
            {
                cxy p(w, h);
                if (!p.isInside(myPolygon))
                    continue;
            }
            cOCell *c = A->cell(w, h);
            c->myType = 2;
            vN.push_back(c);
        }
    if (!vN.size())
        throw std::runtime_error(
            "No unobstructed points");
}