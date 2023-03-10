#pragma once
#include <autocell.h>
#include "cxy.h"
#include "cGraph.h"

/// @brief 2D grid cell that can contain obstacles

class cOCell : public cell::cCell
{
public:
    cOCell()
        : cell::cCell(),
          myType(0),
          fvisited(false)
    {
    }
    /** contents type
     * 0 empty
     * 1 obstacle
     * 2 node that must be visited
     */
    int myType;

    bool fvisited; ///< true if cell has been visited by the robot's path
};

class cObstacle
{

    int nx, ny; ///< grid size

    /**
     * @brief radius of robot's coverage
     *
     * >0 maximum distance, diagonal or orthogonal, in grid points
     * -1 the obstacles are spaced 2 grid points apart, with room for the robot to pass between
     * -999 initial, unset value
     *
     */
    int myView; ///< view radius

    cell::cAutomaton<cOCell> *A; ///< 2D grid
    std::vector<cOCell *> vN;    ///< nodes to be included in path

    raven::graph::cGraph mygraphdata;

    std::vector<std::tuple<std::string, int, int>> myTour;
    int myRevisitedCount;
    int myUnvisitedCount;
    std::vector<cOCell *> myNodesRevisited;

    bool myfrect;               /// true if grid is rectangular
    std::vector<cxy> myPolygon; /// polygon vertices for non-rectangular grid

public:
    cObstacle()
        : myView(-999), myfrect(true)
    {
    }

    void clear();

    /// @brief set grid size
    /// @param x
    /// @param y
    void grid(int x, int y);

    /// @brief set grid to be not rectangular
    void poly()
    {
        myfrect = false;
    }

    /// @brief Add vertex to polgon arounf grid
    /// @param p
    void polyAdd(const cxy &p)
    {
        myPolygon.push_back(p);
    }

    /// @brief set robot view radius
    /// @param v
    void view(int v)
    {
        myView = v;
    }

    /// @brief get robot view radius
    /// @return
    int view() const
    {
        return myView;
    }

    /// @brief Set obstacle at location
    /// @param x
    /// @param y
    void obstacle(int x, int y);

    /// @brief construct nodes to be visited
    void unobstructedPoints();

    void tourNodes();

    /// @brief connect neighbors, avoiding obstacles
    void connect();

    /// @brief display for point at w,h
    /// @param w
    /// @param h
    /// @return display
    std::string draw(int w, int h) const;

    /// @brief get grid dimensions
    /// @param w
    /// @param h
    void size(int &w, int &h) const
    {
        w = nx;
        h = ny;
    }

    /// @brief get grid
    /// @return
    cell::cAutomaton<cOCell> *grid()
    {
        return A;
    }

    /// @brief Is link blocked by obstacle
    /// @param x1,y1 col,row indices
    /// @param y1
    /// @param x2
    /// @param y2
    /// @return
    bool isBlocked(int x1, int y1, int x2, int y2);

    std::vector<std::tuple<std::string, int, int>> tour() const
    {
        if (!myTour.size())
            throw std::runtime_error("No tour");
        return myTour;
    }
    std::vector<cOCell *> NodesRevisited() const
    {
        return myNodesRevisited;
    }

    int unvisitedCount() const
    {
        return myUnvisitedCount;
    }
    int revisitedCount() const
    {
        return myRevisitedCount;
    }

    // vlink_t spanningTree_get()
    // {
    //     return mySpanningTree;
    // }

private:

};

/// @brief read layout of obstacles from file
/// @param cObstacle instance to read into
/// @param fname file name
/// Free function
void read(
    cObstacle &obs,
    const std::string &fname);
