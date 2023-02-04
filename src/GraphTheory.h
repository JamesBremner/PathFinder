#include "cGraphData.h"

std::vector<int>
dijsktra(
    const cGraphData &g,
    const std::string &startName);

std::vector<int>
path(
    const cGraphData &g,
    const std::string &startName,
    const std::string &endName);
