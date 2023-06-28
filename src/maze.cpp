#include <iostream>
#include "cMaze.h"
#include "GraphTheory.h"
#include "cCommandParser.h"

int main(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("help", "\tproduce help message");
    P.add("solve", "fname\tsolve a maze");
    P.add("gen", "bin|rec\tgenerate maze with binary or recursive algorithm");
    P.add("rows", "count\trows in generated maze");
    P.add("cols", "count\tcolumns in generated maze");
    P.add("test1", "\ttest1","bool" );
    P.add("test2", "\ttest2","bool" );
    P.parse(argc, argv);

    std::cout << P.value("test1") << " " << P.value("test2") << "\n";

    auto fname = P.value("solve");
    if (fname.size())
    {
        std::ifstream ifs(fname);
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
        std::cout << M.displayText();
        return 0;
    }
    auto alg = P.value("gen");
    int nrows = atoi(P.value("rows").c_str());
    int ncols = atoi(P.value("cols").c_str());
    if (alg.empty() || nrows == 0 || ncols == 0)
    {
        P.describe();
        return 2;
    }
    raven::graph::cMaze M;
    if (alg[0] == 'b')
        M.generate_binary_tree(nrows, ncols);
    else if (alg[0] = 'r')
        M.generate_recursive_init(nrows, ncols);
    else
    {
        P.describe();
        return 2;
    }
    std::cout << M.displayText();
    return 0;
}