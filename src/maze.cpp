#include <iostream>
#include "cMaze.h"
#include "GraphTheory.h"

int main()
{
    std::ifstream ifs("../dat/maze1.txt");
    if (!ifs.is_open())
    {
        std::cout << "cannot open input file\n";
        return 1;
    }

    raven::graph::cMaze M;
    raven::graph::cGraph g;

    M.read(ifs);
    M.graph(g);
    M.path(path(g, g.startName(), g.endName()).first);
    for (auto &s : M.displayText())
        std::cout << s << "\n";

    return 0;
}