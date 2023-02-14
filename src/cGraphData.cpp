#include <sstream>
#include <algorithm>
#include "cGraphData.h"

void cGraphData::clear()
{
    fDirected = false;
    vVertexName.clear();
    vOutEdges.clear();
    vEdgeDst.clear();
    vVertexAttr.clear();
    vEdgeAttr.clear();
}

void cGraphData::directed(bool f)
{
    fDirected = f;
}

int cGraphData::add(const std::string &vertexName)
{
    int index = find(vertexName);
    if (index >= 0)
        throw std::runtime_error("cGraphData::add duplicate vertex name " + vertexName);
    vVertexName.push_back(vertexName);
    vOutEdges.push_back({});
    return vVertexName.size() - 1;
}
int cGraphData::add(
    const std::string &srcName,
    const std::string &dstName)
{
    return add(find(srcName), find(dstName));
}
int cGraphData::add(int src, int dst,
                    const std::string &sAttr)
{
    int max = vVertexName.size() - 1;
    if (0 > src || src > max ||
        0 > dst || dst > max)
        throw std::runtime_error("cGraphData::add edge vertices out of range");
    int iedge = vEdgeDst.size();
    vOutEdges[src].push_back(iedge);
    vEdgeDst.push_back(dst);
    vEdgeAttr.push_back({});
    vEdgeAttr[iedge].push_back(sAttr);

    if (fDirected)
        return iedge;

    // add reverse edge for undirected graphs
    iedge++;
    vOutEdges[dst].push_back(iedge);
    vEdgeDst.push_back(src);
    vEdgeAttr.push_back({});
    vEdgeAttr[iedge].push_back(sAttr);
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
    return add(
        findorAdd(srcName),
        findorAdd(dstName),
        sAttr);
}
int cGraphData::findorAdd(
    int src,
    int dst,
    const std::string &sAttr)
{
    int ei = add(dst, src, sAttr);
    return ei;
}

void cGraphData::remove(
    const std::string &srcName,
    const std::string &dstName)
{
    remove(
        find(srcName),
        find(dstName));
}

void cGraphData::remove(
    int src,
    int dst)
{
    if (0 > src || src > vertexCount() - 1 ||
        0 > dst || dst > vertexCount() - 1)
        return;
    auto eit = std::find( vOutEdges[src].begin(), vOutEdges[src].end(), dst);
    if( eit != vOutEdges[src].end() ) {
        vEdgeDst[*eit] = -1;
        vOutEdges[src].erase( eit );
    }
    eit = std::find( vOutEdges[dst].begin(), vOutEdges[dst].end(), src);
    if( eit != vOutEdges[dst].end() ) {
        vEdgeDst[*eit] = -1;
        vOutEdges[dst].erase( eit );
    }
}

void cGraphData::edgeAttr(int ie, const std::vector<std::string> &vsAttr)
{
    for (auto &sa : vsAttr)
        vEdgeAttr[ie].push_back(sa);
}
void cGraphData::vertexAttr(int iv, const std::vector<std::string> &vsAttr)
{
    for (auto &sa : vsAttr)
        vVertexAttr[iv].push_back(sa);
}
int cGraphData::edgeCount() const
{
    int c = vEdgeDst.size() - std::count(vEdgeDst.begin(),vEdgeDst.end(),-1);
    if (fDirected)
        return c;
    return c / 2;
}
int cGraphData::find(const std::string &vertexName) const
{
    auto it = std::find(vVertexName.begin(), vVertexName.end(), vertexName);
    if (it == vVertexName.end())
        return -1;
    int i = it - vVertexName.begin();
    return i;
    //return std::distance(vVertexName.begin(), it);
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
    if (0 > vi || vi > vOutEdges.size() - 1)
        throw std::runtime_error("cGraphData::adjacentOut bad vertex index");
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
    if (0 > ai || ai > vEdgeAttr[edg].size() - 1)
        throw std::runtime_error(
            "cGraphData::edgeAttr bad attribute index");
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
            int wi = vEdgeDst[ei];
            if (vi > wi)
                continue;
            ss << "l " << vn << " " << vVertexName[wi];
            for (auto &sa : vEdgeAttr[ei])
                ss << " " << sa;
            ss << "\n";
        }
    }
    return ss.str();
}

std::vector<std::pair<int, int>>
cGraphData::edgeList() const
{
    std::vector<std::pair<int, int>> ret;
    for (int vi = 0; vi < vVertexName.size(); vi++)
    {
        for (int ei : vOutEdges[vi])
        {
            int wi = vEdgeDst[ei];
            if (vi > wi)
                continue;
            ret.push_back(std::make_pair(vi, wi));
        }
    }
    return ret;
}
