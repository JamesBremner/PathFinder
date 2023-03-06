#include <iostream>
#include <sstream>
#include <algorithm>
#include "cVE.h"

namespace raven
{
    namespace graph
    {

        cVE::cVE()
        {
            clear();
        }

        void cVE::clear()
        {
            myVertexCount = 0;
            fDirected = false;
            fDirected = false;
            vOutEdges.clear();
            vInEdges.clear();
            vEdgeDst.clear();
        }

        void cVE::directed(bool f)
        {
            fDirected = f;
        }

        int cVE::add()
        {
            myVertexCount++;
            vOutEdges.resize(myVertexCount);
            vInEdges.resize(myVertexCount);
            return myVertexCount - 1;
        }
        void cVE::add(int i)
        {
            if (i < myVertexCount)
                return;

            // allocate storage for new vertices
            myVertexCount = i + 1;
            vOutEdges.resize(i+1);
            vInEdges.resize(i+1);
        }

        int cVE::add(int src, int dst)
        {
            if (0 > src || 0 > dst)
                throw std::runtime_error("cVE::add edge bad vertex index");

            // ensure there is storage allocated for these vertices
            add( src );
            add( dst);

            int iedge = vEdgeDst.size();
            vOutEdges[src].push_back(iedge);
            vInEdges[dst].push_back(iedge);
            vEdgeDst.push_back(dst);

            if (fDirected)
                return iedge;

            // add reverse edge for undirected graphs
            iedge++;
            vOutEdges[dst].push_back(iedge);
            vEdgeDst.push_back(src);
            return iedge;
        }

        int cVE::findorAdd(
            int src,
            int dst)
        {
            int ei = add(dst, src);
            return ei;
        }

        // void cVE::remove(
        //     const std::string &srcName,
        //     const std::string &dstName)
        // {
        //     remove(
        //         find(srcName),
        //         find(dstName));
        // }

        // void cVE::remove(
        //     int src,
        //     int dst)
        // {
        //     if (0 > src || src > vertexCount() - 1 ||
        //         0 > dst || dst > vertexCount() - 1)
        //         return;

        //     for (auto eit = vOutEdges[src].begin();
        //          eit != vOutEdges[src].end();
        //          eit++)
        //     {
        //         if (vEdgeDst[*eit] == dst)
        //         {
        //             vEdgeDst[*eit] = -1;
        //             vOutEdges[src].erase(eit);
        //             break;
        //         }
        //     }

        //     if (!isDirected())
        //     {
        //         for (auto eit = vOutEdges[dst].begin();
        //              eit != vOutEdges[dst].end();
        //              eit++)
        //         {
        //             if (vEdgeDst[*eit] == src)
        //             {
        //                 vEdgeDst[*eit] = -1;
        //                 vOutEdges[dst].erase(eit);
        //                 break;
        //             }
        //         }
        //     }
        // }

        void cVE::remove(
            int ei)
        {
            // if (0 > ei || ei > vEdgeDst.size() - 1)
            //     return;
            // int dst = vEdgeDst[ei];
            // int src;
            // bool found = false;
            // for (src = 0; src < vVertexName.size(); src++)
            // {
            //     for (auto it = vOutEdges[src].begin(); it != vOutEdges[src].end(); it++)
            //     {
            //         if (*it == ei)
            //         {
            //             vOutEdges[src].erase(it);
            //             for (auto it2 = vInEdges[vEdgeDst[ei]].begin();
            //                  it2 != vInEdges[vEdgeDst[ei]].end();
            //                  it2++)
            //                 if (*it2 == ei)
            //                 {
            //                     vInEdges[vEdgeDst[ei]].erase(it2);
            //                     break;
            //                 }
            //             vEdgeDst[ei] = -1;
            //             found = true;
            //             break;
            //         }
            //     }
            //     if (found)
            //         break;
            // }
            // if (isDirected())
            //     return;
            // for (auto it = vOutEdges[dst].begin();
            //      it != vOutEdges[dst].end();
            //      it++)
            // {
            //     if (vEdgeDst[*it] == src)
            //         vEdgeDst[*it] = -1;
            //     vOutEdges[dst].erase(it);
            //     break;
            // }
        }

        int cVE::edgeCount() const
        {
            int c = vEdgeDst.size() - std::count(vEdgeDst.begin(), vEdgeDst.end(), -1);
            if (fDirected)
                return c;
            return c / 2;
        }
        // int cVE::find(const std::string &vertexName) const

        int cVE::find(int src, int dst) const
        {
            for (int ei : vOutEdges[src])
            {
                if (vEdgeDst[ei] == dst)
                    return ei;
            }
            return -1;
        }

        std::vector<int> cVE::adjacentOut(int vi) const
        {
            if (0 > vi || vi > vOutEdges.size() - 1)
                throw std::runtime_error("cVE::adjacentOut bad vertex index");
            std::vector<int> ret;
            for (int ei : vOutEdges[vi])
                ret.push_back(vEdgeDst[ei]);
            return ret;
        }

        std::vector<int> cVE::adjacentIn(int vi) const
        {
            std::vector<int> ret;
            for (int ei : vInEdges[vi])
                ret.push_back(vEdgeDst[ei]);
            return ret;
        }

        // std::string cVE::userName(int i) const
        // {
        //     std::string ret = vVertexName[i];
        //     if (ret == "@myIndex")
        //         return std::to_string(i);
        //     return ret;
        // }

        // std::vector<std::string> cVE::userName(std::vector<int> vi) const
        // {
        //     std::vector<std::string> ret;
        //     for (int i : vi)
        //         ret.push_back(userName(i));
        //     return ret;
        // }

        // std::vector<std::string> cVE::vertexNames() const
        // {
        //     std::vector<std::string> ret;
        //     for (int kv = 0; kv < vVertexName.size(); kv++)
        //     {
        //         if (vVertexName[kv] == "@myIndex")
        //             ret.push_back(std::to_string(kv));
        //         else
        //             ret.push_back(vVertexName[kv]);
        //     }
        //     return ret;
        // }

        // std::string cVE::text() const
        // {
        //     std::stringstream ss;
        //     for (auto &vn : vVertexName)
        //     {
        //         int vi = find(vn);
        //         for (int ei : vOutEdges[vi])
        //         {
        //             int wi = vEdgeDst[ei];
        //             if ((!fDirected) && (vi > wi))
        //                 continue;
        //             ss << "l " << vn << " " << vVertexName[wi];
        //             ss << "\n";
        //         }
        //     }
        //     return ss.str();
        // }

        std::vector<std::pair<int, int>>
        cVE::edgeList() const
        {
            std::vector<std::pair<int, int>> ret;
            for (int vi = 0; vi < myVertexCount; vi++)
            {
                for (int ei : vOutEdges[vi])
                {
                    int wi = vEdgeDst[ei];
                    if (wi < 0)
                        continue;
                    if (vi > wi)
                        continue;
                    ret.push_back(std::make_pair(vi, wi));
                }
            }
            return ret;
        }

    }
}
