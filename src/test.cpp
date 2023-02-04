#include  <iostream>
#include "GraphTheory.h"

main()
{
    cGraphData g;
    g.findorAdd("a","b","1");
    g.findorAdd("b","c","1");
    std::cout << g.text();

    auto p = path( g, "a", "c" );
}