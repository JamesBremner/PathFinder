#include <iostream>
#include "cMaze.h"
#include "GraphTheory.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: maze <maze filename>\n";
        return 2;
    }
    std::ifstream ifs(argv[1]);
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