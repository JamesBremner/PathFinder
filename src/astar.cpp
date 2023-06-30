#include <iostream>
#include <functional>
#include "cGraph.h"
namespace raven
{
    namespace graph
    {
class cAstarFringe
{
    /* The fringe
     The vertices that are waiting to be extended
     stored along with their estimated total cost to reach the goal using the vertex in the path
     sorted so the vertex with lowest estimated store is first

     This functions as a pritory queue
     The STL priority queue is not used because it does not provide
     a simple way of checking that a vertex is or is not present in the queue
     */
    std::vector<std::pair<int, double>> myFringe;

public:

    int size() const
    {
        return myFringe.size();
    }
    /// @brief Add vertex to fringe
    /// @param vertexIndex 
    /// @param fscore The estimated total cost to the goal through this vertex

    void push(int vertexIndex, double fscore)
    {
        myFringe.push_back(std::make_pair(vertexIndex, fscore));

        // sort so the vertex with lowest estimated store is first
        std::sort(
            myFringe.begin(),
            myFringe.end(),
            [this](std::pair<int, double> &lhs, std::pair<int, double> &rhs) -> bool
            {
                return lhs.second < rhs.second;
            });
    }
    /// @brief get index of fringe vertex with lowest estimated total cost to the goal
    /// @return 
    int top() const
    {
        return myFringe[0].first;
    }
    void pop()
    {
        myFringe.erase(myFringe.begin());
    }

    /// @brief true if vertex in fringe
    /// @param vi 
    /// @return 
    bool contains(int vi) const
    {
        auto it = std::find_if(
            myFringe.begin(), myFringe.end(),
            [&](const std::pair<int, double> &p) -> bool
            {
                return (p.first == vi);
            });
        return (it != myFringe.end());
    }

    /// @brief get optimum path found to current node
    /// @param[in] pred vector of vertex predeccesors
    /// @param current 
    /// @return 
    std::vector<int> getPath(
        const std::vector<int> &pred,
        int current) const
    {
        std::vector<int> path;
        while (current != -1)
        {
            path.push_back(current);
            current = pred[current];
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

std::vector<int> astar(
    raven::graph::cGraph &g,
    std::function<double(int)> edgeWeight,
    int start, int goal,
    std::function<double(int)> heuristic)
{
    if( start < 0 || goal < 0 )
        throw std::runtime_error(
            "astar bad vertex index"        );

    std::vector<int> path;
    std::vector<int> cameFrom(g.vertexCount(), -1);
    std::vector<double> gScore(g.vertexCount(), INT_MAX);
    cAstarFringe fringe;
    bool success = false;
    int current;

    gScore[start] = 0;
    fringe.push(start,heuristic(start));
    while (fringe.size())
    {
        /// get node in fringe with the best estimated score
        current = fringe.top();
        if (current == goal)
        {
            success = true;
            break;
        }
        fringe.pop();

        /// loop over neighbors
        for (int neighbor : g.adjacentOut(current))
        {
            // score so far to reach neighbor
            double tentative_gScore =
                gScore[current] + edgeWeight(g.find(current, neighbor));
            
            // check if this is the cheapest way to reach neighbor
            if (tentative_gScore < gScore[neighbor])
            {
                // update predeccesor
                cameFrom[neighbor] = current;

                // update score
                gScore[neighbor] = tentative_gScore;

                // if not already in fringe, add
                if (!fringe.contains(neighbor))
                    fringe.push(
                        neighbor,
                        tentative_gScore + heuristic(neighbor) );
            }
        }
    }
    if (success)
        path = fringe.getPath(cameFrom, goal);

    return path;
}
/// @brief Inplement the A* algorithm
/// @param g graph to be searched
/// @param vEdgeWeight constant graph edge weights
/// @param start vertex index
/// @param goal vertex index
/// @param heuristic
/// @return vector of vertex indices on path from start to goal
std::vector<int> astarDynWeights(
    raven::graph::cGraph &g,
    std::function<double(int, const std::vector<int> &)> dynWeight,
    int start, int goal,
    std::function<double(int)> heuristic)
{
    std::vector<int> path;
    std::vector<int> cameFrom(g.vertexCount(), -1);
    std::vector<double> gScore(g.vertexCount(), INT_MAX);
    std::vector<double> fScore(g.vertexCount(), INT_MAX);
    cAstarFringe fringe;
    bool success = false;
    int current;

    gScore[start] = 0;
    fScore[start] = heuristic(start);
    fringe.push(start,fScore[start] );
    while (fringe.size())
    {
        current = fringe.top();
        if (current == goal)
        {
            success = true;
            break;
        }
        fringe.pop();
        for (int neighbor : g.adjacentOut(current))
        {
            double tentative_gScore =
                gScore[current] + dynWeight(
                                      g.find(current, neighbor),
                                      fringe.getPath(cameFrom, current));

            if (tentative_gScore < gScore[neighbor])
            {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = tentative_gScore + heuristic(neighbor);
                if (!fringe.contains(neighbor))
                    fringe.push(neighbor,fScore[neighbor]);
            }
        }
    }
    if (success)
        path = fringe.getPath(cameFrom, current);

    return path;
}
    }
}