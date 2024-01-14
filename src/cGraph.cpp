
#include <iostream>
#include <sstream>
#include <algorithm>
#include "cGraph.h"

namespace raven
{
    namespace graph
    {
        cGraph::cGraph()
        {
            clear();
        }
        void cGraph::clear()
        {
            fDirected = false;
            vOutVertices.clear();
            vInVertices.clear();
            vVertexAttr.clear();
            vVertexName.clear();
            mapEdge.clear();
            lastEdgeIndex = -1;
        }
        int cGraph::add(const std::string &name)
        {
            if (std::find(vVertexName.begin(), vVertexName.end(), name) != vVertexName.end())
                throw std::runtime_error(
                    "cGraph::add duplicate vertex name");
            vOutVertices.push_back({});
            vInVertices.push_back({});
            vVertexName.push_back(name);
            vVertexAttr.push_back({});
            return vVertexName.size() - 1;
        }

        int cGraph::add(const std::string &src, const std::string &dst)
        {
            // find or add the vertices
            int s = find(src);
            if (s == -1)
                s = add(src);
            int d = find(dst);
            if (d == -1)
                d = add(dst);

            return add(s, d);
        }
        int cGraph::add(int s, int d)
        {
            if (s < 0 || d < 0)
                throw std::runtime_error(
                    "cGraph::add( int s, int d ) bad vertex index");

            // vertex existence check
            if (s > vertexCount() - 1 || d > vertexCount() - 1)
            {
                int max = s;
                if (d > max)
                    max = d;
                for (int kv = vertexCount(); kv <= max; kv++)
                    add("V" + std::to_string(kv));
            }

            // duplicate edge check
            int ei = find(s, d);
            if (ei != -1)
                return ei;

            // add edge
            vOutVertices[s].push_back(d);
            vInVertices[d].push_back(s);
            lastEdgeIndex++;
            mapEdge.insert(mapEdge_t::value_type(std::make_pair(s, d), lastEdgeIndex));

            if (fDirected)
                return lastEdgeIndex;

            // add reverse edge
            vOutVertices[d].push_back(s);
            vInVertices[s].push_back(d);
            lastEdgeIndex++;
            mapEdge.insert(mapEdge_t::value_type(std::make_pair(d, s), lastEdgeIndex));

            return lastEdgeIndex - 1;
        }

        void cGraph::wVertexAttr(int vi, const std::vector<std::string> vAttr)
        {
            if (0 > vi || vi >= vVertexAttr.size())
                throw std::runtime_error(
                    "cGraph::wVertexAttr bad vertex index");
            vVertexAttr[vi] = vAttr;
        }

        void cGraph::remove(int s, int d)
        {
            // clear edge attributes
            int ei = find(s, d);
            if (ei < 0)
            {
                // edge attributes do not exist
                // assume this means the edge does not exist
                // and no work is needed
                return;
            }

            // remove from src out edges
            auto it = std::find(vOutVertices[s].begin(), vOutVertices[s].end(), d);
            if (it != vOutVertices[s].end())
                vOutVertices[s].erase(it);

            // remove from dst in edges
            it = std::find(vInVertices[d].begin(), vInVertices[d].end(), s);
            if (it != vInVertices[d].end())
                vInVertices[d].erase(it);

            mapEdge.erase(mapEdge.find(std::make_pair(s, d)));

            if (fDirected)
                return;

            it = std::find(vOutVertices[d].begin(), vOutVertices[d].end(), s);
            if (it != vOutVertices[d].end())
                vOutVertices[d].erase(it);
            it = std::find(vInVertices[s].begin(), vInVertices[s].end(), d);
            if (it != vInVertices[s].end())
                vInVertices[s].erase(it);
            mapEdge.erase(mapEdge.find(std::make_pair(d, s)));
        }
        void cGraph::remove(const std::string &src, const std::string &dst)
        {
            remove(find(src), find(dst));
        }

        void cGraph::remove(int removed)
        {
            for (int a : adjacentOut(removed))
                remove(removed, a);
            vOutVertices.erase(vOutVertices.begin() + removed);
            vInVertices.erase(vInVertices.begin() + removed);
            vVertexName.erase(vVertexName.begin() + removed);
            vVertexAttr.erase(vVertexAttr.begin() + removed);
        }

        int cGraph::vertexCount() const
        {
            return vOutVertices.size();
        }
        int cGraph::edgeCount() const
        {
            int ret = 0;
            for (auto &vo : vOutVertices)
                ret += vo.size();
            if (!fDirected)
                ret /= 2;
            return ret;
        }
        int cGraph::find(const std::string &name) const
        {
            auto it = std::find(vVertexName.begin(), vVertexName.end(), name);
            if (it == vVertexName.end())
                return -1;
            return it - vVertexName.begin();
        }
        int cGraph::find(int s, int d) const
        {
            if (!fDirected)
            {
                if (s > d)
                {
                    int t = s;
                    s = d;
                    d = t;
                }
            }
            auto it = mapEdge.find(std::make_pair(s, d));
            if (it == mapEdge.end())
                return -1;
            return it->second;
        }
        int cGraph::find(const std::string &src, const std::string &dst) const
        {
            return find(find(src), find(dst));
        }
        std::string cGraph::userName(int vi) const
        {
            return vVertexName[vi];
        }
        std::vector<std::string> cGraph::userName(std::vector<int> vvi) const
        {
            std::vector<std::string> ret;
            for (int i : vvi)
                ret.push_back(userName(i));
            return ret;
        }
        std::vector<int> cGraph::adjacentOut(int vi) const
        {
            return vOutVertices[vi];
        }
        std::vector<int> cGraph::adjacentIn(int vi) const
        {
            return vInVertices[vi];
        }

        int cGraph::dest(int ei) const
        {
            auto it = mapEdge.begin();
            for (; it != mapEdge.end(); it++)
                if (it->second == ei)
                    return it->first.second;

            return -1;
        }
        int cGraph::src(int ei) const
        {
            auto it = mapEdge.begin();
            for (; it != mapEdge.end(); it++)
                if (it->second == ei)
                    return it->first.first;

            return -1;
        }
        std::string cGraph::rVertexAttr(int vi, int ai) const
        {
            if (ai >= vVertexAttr[vi].size())
                return "";
            return vVertexAttr[vi][ai];
        }

        std::vector<std::pair<int, int>>
        cGraph::edgeList() const
        {
            std::vector<std::pair<int, int>> ret;
            int src = -1;
            for (auto &ve : vOutVertices)
            {
                src++;
                for (int dst : ve)
                {
                    if (!isDirected())
                        if (src >= dst)
                            continue;
                    ret.push_back(std::make_pair(src, dst));
                }
            }
            return ret;
        }
        std::string cGraph::text() const
        {
            std::stringstream ss;
            for (auto &p : edgeList())
                ss << vVertexName[p.first] << " - " << vVertexName[p.second] << "\n";
            return ss.str();
        }
    }
}