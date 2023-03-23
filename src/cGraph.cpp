
#include <iostream>
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
            vOutEdges.clear();
            vInEdges.clear();
            vVertexAttr.clear();
            vVertexName.clear();
            mapEdgeAttr.clear();
            vEdgeAttr.clear();
        }
        int cGraph::add(const std::string &name)
        {
            if (std::find(vVertexName.begin(), vVertexName.end(), name) != vVertexName.end())
                throw std::runtime_error(
                    "cGraph::add duplicate vertex name");
            vOutEdges.push_back({});
            vInEdges.push_back({});
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
            vOutEdges[s].push_back(d);
            vInEdges[d].push_back(s);
            mapEdgeAttr.insert(mapEdgeAttr_t::value_type(std::make_pair(s, d), vEdgeAttr.size()));
            vEdgeAttr.push_back({});

            if (fDirected)
                return vEdgeAttr.size() - 1;

            // add reverse edge
            vOutEdges[d].push_back(s);
            vInEdges[s].push_back(d);
            mapEdgeAttr.insert(mapEdgeAttr_t::value_type(std::make_pair(d, s), vEdgeAttr.size()));
            vEdgeAttr.push_back({});

            return vEdgeAttr.size() - 2;
        }

        void cGraph::wVertexAttr(int vi, const std::vector<std::string> vAttr)
        {
            if (0 > vi || vi > vVertexAttr.size() - 1)
                throw std::runtime_error(
                    "cGraph::wVertexAttr bad vertex index");
            vVertexAttr[vi] = vAttr;
        }
        void cGraph::wEdgeAttr(int ei, const std::vector<std::string> vAttr)
        {
            vEdgeAttr[ei] = vAttr;
        }
        void cGraph::wEdgeAttr(int s, int d, const std::vector<std::string> vAttr)
        {
            wEdgeAttr(find(s, d), vAttr);
        }

        void cGraph::remove(int s, int d)
        {
            auto it = std::find(vOutEdges[s].begin(), vOutEdges[s].end(), d);
            if (it != vOutEdges[s].end())
                vOutEdges[s].erase(it);
            it = std::find(vInEdges[d].begin(), vInEdges[d].end(), s);
            if (it != vInEdges[d].end())
                vInEdges[d].erase(it);
            vEdgeAttr[find(s, d)] = {};
            if (fDirected)
                return;
            it = std::find(vOutEdges[d].begin(), vOutEdges[d].end(), s);
            if (it != vOutEdges[d].end())
                vOutEdges[d].erase(it);
            it = std::find(vInEdges[s].begin(), vInEdges[s].end(), d);
            if (it != vInEdges[s].end())
                vInEdges[s].erase(it);
            vEdgeAttr[find(d, s)] = {};
        }
        void cGraph::remove(const std::string &src, const std::string &dst)
        {
            remove(find(src), find(dst));
        }

        int cGraph::vertexCount() const
        {
            return vOutEdges.size();
        }
        int cGraph::edgeCount() const
        {
            int ret = 0;
            for (auto &vo : vOutEdges)
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
            try
            {
                return mapEdgeAttr.at(std::make_pair(s, d));
            }
            catch (...)
            {
                return -1;
            }
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
            return vOutEdges[vi];
        }
        std::vector<int> cGraph::adjacentIn(int vi) const
        {
            return vInEdges[vi];
        }

        int cGraph::dest(int ei) const
        {
            auto it = mapEdgeAttr.begin();
            for (; it != mapEdgeAttr.end(); it++)
                if (it->second == ei)
                    return it->first.second;

            return -1;
        }
        int cGraph::src(int ei) const
        {
            auto it = mapEdgeAttr.begin();
            for (; it != mapEdgeAttr.end(); it++)
                if (it->second == ei)
                    return it->first.first;

            return -1;
        }
        std::string cGraph::rVertexAttr(int vi, int ai) const
        {
            return vVertexAttr[vi][ai];
        }
        std::string cGraph::rEdgeAttr(int ei, int ai) const
        {
            if (0 > ai || ai > (int)vEdgeAttr[ei].size() - 1)
                return "";
            return vEdgeAttr[ei][ai];
        }

        std::vector<std::pair<int, int>>
        cGraph::edgeList() const
        {
            std::vector<std::pair<int, int>> ret;
            int src = -1;
            for (auto &ve : vOutEdges)
            {
                src++;
                for (int dst : ve)
                {
                    if (src >= dst)
                        continue;
                    ret.push_back(std::make_pair(src, dst));
                }
            }
            return ret;
        }
    }
}