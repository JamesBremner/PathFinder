#include  <iostream>
#include "cGraphData.h"

main()
{
    cGraphData g;
    g.add("a");
    g.add("b");
    g.add("a","b");
    std::cout << g.text();
}