#include <iostream>
#include <sstream>
#include <algorithm>
#include "cGraphData.h"

namespace raven
{

    cGraphData::cGraphData()
    {
        clear();
    }

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
        vInEdges.push_back({});
        vVertexAttr.push_back({});
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
        if (0 > src || 0 > dst)
            throw std::runtime_error("cGraphData::add edge bad vertex index");

        // check there is storage allocated for these vertices
        int max = vVertexName.size() - 1;
        if (src > max || dst > max)
        {

            // allocate storage for new vertices
            // the new vertex names are set to '@myIndex'
            // which indicates the name is the index formatted as a string
            max = src;
            if (dst > src)
                max = dst;
            max++;
            vVertexName.resize(max, "@myIndex");
            vOutEdges.resize(max);
            vInEdges.resize(max);
        }

        int iedge = vEdgeDst.size();
        vOutEdges[src].push_back(iedge);
        vInEdges[dst].push_back(iedge);
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

        for (auto eit = vOutEdges[src].begin();
             eit != vOutEdges[src].end();
             eit++)
        {
            if (vEdgeDst[*eit] == dst)
            {
                vEdgeDst[*eit] = -1;
                vOutEdges[src].erase(eit);
                break;
            }
        }

        if (!isDirected())
        {
            for (auto eit = vOutEdges[dst].begin();
                 eit != vOutEdges[dst].end();
                 eit++)
            {
                if (vEdgeDst[*eit] == src)
                {
                    vEdgeDst[*eit] = -1;
                    vOutEdges[dst].erase(eit);
                    break;
                }
            }
        }
    }

    void cGraphData::wEdgeAttr(int ie, const std::vector<std::string> &vsAttr)
    {
        if (0 > ie || ie > vEdgeAttr.size() - 1)
            throw std::runtime_error(
                "cGraphData::eEdgAttr bad index");
        vEdgeAttr[ie] = vsAttr;
    }
    void cGraphData::wVertexAttr(int iv, const std::vector<std::string> &vsAttr)
    {
        if (0 > iv || iv > vVertexAttr.size() - 1)
            throw std::runtime_error(
                "cGraphData::vertexAttr bad index");
        vVertexAttr[iv] = vsAttr;
    }
    int cGraphData::edgeCount() const
    {
        int c = vEdgeDst.size() - std::count(vEdgeDst.begin(), vEdgeDst.end(), -1);
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
        // return std::distance(vVertexName.begin(), it);
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

    std::vector<std::string> cGraphData::adjacentOut(const std::string &name) const
    {
        std::vector<std::string> ret;
        for (int i : adjacentOut(find(name)))
            ret.push_back(vVertexName[i]);
        return ret;
    }

    std::vector<int> cGraphData::adjacentIn(int vi) const
    {
        std::vector<int> ret;
        for (int ei : vInEdges[vi])
            ret.push_back(vEdgeDst[ei]);
        return ret;
    }

    double cGraphData::rVertexAttr(int vi, int ai) const
    {
        auto s = rVertexAttrString(vi, ai);
        if (s.empty())
            return INT_MAX;
        return atof(s.c_str());
    }
    std::string cGraphData::rVertexAttrString(int vi, int ai) const
    {
        if (0 > vi || vi > (int)vVertexName.size() - 1)
            return "";
        if (0 > ai || ai > (int)vVertexAttr[vi].size() - 1)
            return "";
        return vVertexAttr[vi][ai];
    }

    double cGraphData::rEdgeAttr(int src, int dst, int ai) const
    {
        return rEdgeAttr(find(src, dst), ai);
    }
    double cGraphData::rEdgeAttr(int ei, int ai) const
    {
        if (ei < 0)
            return INT_MAX;
        if (0 > ai || ai > vEdgeAttr[ei].size() - 1)
            throw std::runtime_error(
                "cGraphData::edgeAttr bad attribute index");
        return atof(vEdgeAttr[ei][ai].c_str());
    }

    std::string cGraphData::userName(int i) const
    {
        std::string ret = vVertexName[i];
        if (ret == "@myIndex")
            return std::to_string(i);
        return ret;
    }

    std::vector<std::string> cGraphData::userName(std::vector<int> vi) const
    {
        std::vector<std::string> ret;
        for (int i : vi)
            ret.push_back(userName(i));
        return ret;
    }

    std::vector<std::string> cGraphData::vertexNames() const
    {
        std::vector<std::string> ret;
        for (int kv = 0; kv < vVertexName.size(); kv++)
        {
            if (vVertexName[kv] == "@myIndex")
                ret.push_back(std::to_string(kv));
            else
                ret.push_back(vVertexName[kv]);
        }
        return ret;
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
                if (( ! fDirected ) && (vi > wi))
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

}
