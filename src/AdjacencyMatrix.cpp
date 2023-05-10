#include <vector>
#include "cGraph.h"

std::vector<std::vector<int>>
makeAdjacencyMatrix(const raven::graph::cGraph &g)
{
    std::vector<std::vector<int>> M;
    int N = g.vertexCount();
    for (int n1 = 0; n1 < N; n1++)
    {
        std::vector<int> row;
        for (int n2 = 0; n2 < N; n2++)
        {
            if (n1 == n2)
                row.push_back(0);
            else
                row.push_back(edgeWeight(g, n1, n2));
        }
        M.push_back(row);
    }
    return M;
}

raven::graph::cGraph
makeGraph(int adj[N][N])
{
    raven::graph::cGraph g;
    for (int n1 = 0; n1 < N; n1++)
    {
        for (int n2 = n1 + 1; n2 < N; n2++)
        {
            g.wEdgeAttr(
                g.add(n1, n2),
                {std::to_string(adj[n1][n2])});
        }
    }
    return g;
}