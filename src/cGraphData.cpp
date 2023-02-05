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
    return add(isrc, idst);
}
int cGraphData::add(int src, int dst)
{
    int max = vVertexName.size() - 1;
    if (0 > src || src > max ||
        0 > dst || dst > max)
        throw std::runtime_error("cGraphData::add out of range");
    int iedge = vEdgeDst.size();
    vOutEdges[src].push_back(iedge);
    vEdgeDst.push_back(dst);
    vEdgeAttr.push_back({});
    return iedge;
}

int cGraphData::findorAdd(const std::string vertexName)
{
    int index = find(vertexName);
    if (index < 0)
        index = add(vertexName);
    return index;
}

int cGraphData::findorAdd(
    const std::string &srcName,
    const std::string &dstName,
    const std::string &sAttr)
{
    return findorAdd(
        findorAdd(srcName),
        findorAdd(dstName),
        sAttr);

}
int cGraphData::findorAdd(
    int src,
    int dst,
    const std::string &sAttr)
{
    int edg = add(src, dst);
    vEdgeAttr[edg].push_back(sAttr);
    return edg;
}
int cGraphData::find(const std::string &vertexName) const
{
    auto it = std::find(vVertexName.begin(), vVertexName.end(), vertexName);
    if (it == vVertexName.end())
        return -1;
    return std::distance(vVertexName.begin(), it);
}

int cGraphData::find(int src, int dst) const
{
    for (int ei : vOutEdges[src])
    {
        if (vEdgeDst[ei] == dst)
            return ei;
    }
    return -1;
}

std::vector<int> cGraphData::adjacentOut(int vi) const
{
    std::vector<int> ret;
    for (int ei : vOutEdges[vi])
        ret.push_back(vEdgeDst[ei]);
    return ret;
}
double cGraphData::edgeAttr(int src, int dst, int ai) const
{
    int edg = find(src, dst);
    if (edg < 0)
        return INT_MAX;
    return atof(vEdgeAttr[edg][ai].c_str());
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
