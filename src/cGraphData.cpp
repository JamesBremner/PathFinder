#include <sstream>
#include <algorithm>
#include "cGraphData.h"

int cGraphData::add(const std::string &vertexName)
{
    int index = find(vertexName);
    if (index >= 0)
        throw std::runtime_error("cGraphData::add duplicate vertex name " + vertexName);
    vVertexName.push_back(vertexName);
    vOutEdges.push_back({});
    return vVertexName.size() - 1;
}
int cGraphData::add(const std::string &srcName, const std::string &dstName)
{
    int isrc = find(srcName);
    int idst = find(dstName);
    if (isrc < 0 || idst < 0)
        throw std::runtime_error("cGraphData::add missing vertex name " + srcName + " or " + dstName);
    int iedge = vEdgeDst.size();
    vOutEdges[isrc].push_back(iedge);
    vEdgeDst.push_back(idst);
    return iedge;
}

int cGraphData::find(const std::string &vertexName) const
{
    auto it = std::find(vVertexName.begin(), vVertexName.end(), vertexName);
    if (it == vVertexName.end())
        return -1;
    return std::distance(vVertexName.begin(), it);
}

std::string cGraphData::text() const
{
    std::stringstream ss;
    for (auto &vn : vVertexName)
    {
        int vi = find(vn);
        for (int ei : vOutEdges[vi])
        {
            ss << vn << " - " << vVertexName[vEdgeDst[ei]] << "\n";
        }
    }
    return ss.str();
}
