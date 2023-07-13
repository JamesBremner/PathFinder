#include <iostream>
#include <vector>
#include "GraphTheory.h"

namespace raven
{
    namespace graph
    {
// find articulation points in an undirected graph


// A recursive function that find articulation
// points using DFS traversal
// adj[] --> Adjacency List representation of the graph
// u --> The vertex to be visited next
// visited[] --> keeps track of visited vertices
// disc[] --> Stores discovery times of visited vertices
// low[] -- >> earliest visited vertex (the vertex with minimum
// discovery time) that can be reached from subtree
// rooted with current vertex
// parent --> Stores the parent vertex in DFS tree
// isAP[] --> Stores articulation points
static void tarjanUtil(sGraphData& gd,  std::vector<bool>& visited,
            std::vector<int>& disc, std::vector<int>& low, int& time, int parent,
            std::vector<bool> isAP)
{
    int u = gd.g.find(gd.startName);

    // Count of children in DFS Tree
    int children = 0;
 
    // Mark the current node as visited
    visited[u] = true;
 
    // Initialize discovery time and low value
    disc[u] = low[u] = ++time;
 
    // Go through all vertices adjacent to this
    for (auto v : gd.g.adjacentOut( u ) ) {
        // If v is not visited yet, then make it a child of u
        // in DFS tree and recur for it
        if (!visited[v]) {
            children++;
            sGraphData gd2;
            gd2 = gd;
            gd2.startName= gd.g.userName( v );
            tarjanUtil(gd, visited, disc, low, time, u, isAP);
 
            // Check if the subtree rooted with v has
            // a connection to one of the ancestors of u
            low[u] = std::min(low[u], low[v]);
 
            // If u is not root and low value of one of
            // its child is more than discovery value of u.
            if (parent != -1 && low[v] >= disc[u])
                isAP[u] = true;
        }
 
        // Update low value of u for parent function calls.
        else if (v != parent)
            low[u] = std::min(low[u], disc[v]);
    }
 
    // If u is root of DFS tree and has two or more children.
    if (parent == -1 && children > 1)
        isAP[u] = true;
}
 
void tarjan(sGraphData& gd )
{
    std::vector<int> disc( gd.g.vertexCount(),0);
    std::vector<int> low( gd.g.vertexCount());
    std::vector<bool> visited( gd.g.vertexCount(), false );
    std::vector<bool> isAP( gd.g.vertexCount(), false );
    int time = 0, par = -1;
 
    // Adding this loop so that the
    // code works even if we are given
    // disconnected graph
    for (int u = 0; u <  gd.g.vertexCount(); u++)
        if (!visited[u]) {
            gd.startName = gd.g.userName(u);
            tarjanUtil(gd, visited, disc, low,
                   time, par, isAP);
        }
 
    // Printing the APs
    for (int u = 0; u < gd.g.vertexCount(); u++)
        if (isAP[u])
            std::cout << u << " ";
}
    }
}
 