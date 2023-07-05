#pragma once
#include <vector>
#include <map>
#include <string>

namespace raven
{
    namespace graph
    {
        class cGraph
        {
        public:
            cGraph();

            void clear();

            void directed(bool f = true)
            {
                fDirected = f;
            }

            /// @brief add a new vertex
            /// @param name
            /// @return vertex index

            int add(const std::string &name);

            /* @brief add a new edge between named vertices
            /// @param scr name
            /// @param dst name
            /// @return edge index
            If vertices do not exist they will be added
            If edge exists no edge will be added and existing edge index returned
            */
            int add(const std::string &scr, const std::string &dst);

            /* @brief add a new edge between vertex indices
            /// @param s
            /// @param d
            /// @return edge index

            If the vertices do not exist, then new vertices are added named V(vertex index)

            This is faster than the named vertex version
            because it does no sanity checking.
            It should only be used for constructing large graphs
            where sanity can be assumed and performance is important
            */
            int add(int s, int d);

            void wVertexAttr(int vi, const std::vector<std::string> vAttr);

            /* @brief remove an edge
            /// @param s
            /// @param d

            The edge is removed from the source out edges and the destination in edges ( directed graph )
            */
            void remove(int s, int d);
            void remove(const std::string &str, const std::string &dst);

            /// remove vertex
            void remove(int removed);

            /////////////////////// getters

            bool isDirected() const { return fDirected; }
            int vertexCount() const;
            int edgeCount() const;

            int find(const std::string &name) const;
            int find(int s, int d) const;
            int find(const std::string &src, const std::string &dst) const;

            std::string userName(int vi) const;
            std::vector<std::string> userName(std::vector<int> vvi) const;

            std::vector<int> adjacentOut(int vi) const;
            std::vector<int> adjacentIn(int vi) const;

            int dest(int ei) const;
            int src(int ei) const;

            std::string rVertexAttr(int vi, int ai) const;

            /// @brief edges in graph
            /// @return vector of vertex index pairs for vertices that are connected

            std::vector<std::pair<int, int>>
            edgeList() const;

            std::string text() const;

        private:
            bool fDirected;


            // edges

            std::vector<std::vector<int>> vOutEdges;

            std::vector<std::vector<int>> vInEdges;

            /** vertex attributes
             *
             *  vVertexAttr[vi][ai] is a string representing the aith attribute of the vith vertex
             */

            std::vector<std::string> vVertexName;
            std::vector<std::vector<std::string>> vVertexAttr;

            /* edge map

                mapEdgeAttr[src,dst]] is the index of the edge src -> dst
            */
            int lastEdgeIndex;
            typedef std::map<std::pair<int, int>, int> mapEdge_t;
            mapEdge_t mapEdge;
        };

    }
}