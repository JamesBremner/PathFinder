#pragma once
#include "cVE.h"
#include "cAttribute.h"

namespace raven
{
    namespace graph
    {
        /// @brief A graph of vertices connected by edges, with vertex and edge attribute

        class cGraph : public cVE
        {
        public:
            // writers

            void clear()
            {
                cVE::clear();
                myAtt.clear();
            }

            void wVertexName( int iv, const std::string& name )
            {
                myAtt.wVertexName(iv,name);
            }
            void wVertexAttr(
                int vi,
                const std::vector<std::string> &vsAttr)
            {
                myAtt.wVertexAttr(vi, vsAttr);
            }
            void wEdgeAttr(
                int ei,
                const std::vector<std::string> &vsAttr)
            {
                myAtt.wEdgeAttr(ei, vsAttr);
            }
            void wEdgeAttr(
                int v1, int v2,
                const std::vector<std::string> &vsAttr)
            {
                myAtt.wEdgeAttr(cVE::find(v1, v2), vsAttr);
                if (!isDirected())
                    myAtt.wEdgeAttr(cVE::find(v2, v1), vsAttr);
            }

            int add(const std::string &name)
            {
                myAtt.add(name);
                return cVE::add();
            }

            /// @brief add edge, add required vertices
            /// @param srcName
            /// @param dstName
            /// @return edge index
            int findorAdd(
                const std::string &srcName,
                const std::string &dstName)
            {
                int s = find(srcName);
                if (s < 0)
                    s = add(srcName);
                int d = find(dstName);
                if (d < 0)
                    d = add(dstName);
                return cVE::findorAdd( s, d );
            }

            int findorAdd(int s, int d)
            {
                cVE::add(s);
                cVE::add(d);
                myAtt.resizeVertex( vertexCount() );
                return cVE::findorAdd(s, d);
             }


            ////////// readers

            /// @brief read vertex attribute
            /// @param vi vertex index
            /// @param ai attribute index
            /// @return attribute in string format

            std::string rVertexAttr(int vi, int ai) const
            {
                return myAtt.rVertexAttr(vi, ai);
            }

            /// @brief read edge attribute
            /// @param ei edge index
            /// @param ai attribute index
            /// @return attribute in string format

            std::string rEdgeAttr(int ei, int ai) const
            {
                return myAtt.rEdgeAttr(ei, ai);
            }

            /// @brief vertex index from name
            /// @param name 
            /// @return vertex index

            int find(const std::string &name) const
            {
                return myAtt.find(name);
            }

            /// @brief vertex name from index
            /// @param i vertex index
            /// @return vertex name: "<index>" if unnamed, "" if bad index

            std::string userName(int i) const
            {
                if( 0 > i || i >= vertexCount() )
                    return "";
                std::string s = myAtt.userName(i);
                if( s.empty() )
                    s = std::to_string( i );
                return s;
            }

            /// @brief edge index from vertex indices
            /// @param s source vertex index
            /// @param d destination vertex index
            /// @return edge index

            int find(int s, int d) const
            {
                return cVE::find(s, d);
            }

            /// @brief vertex user names
            /// @param vi vector of vertex indices
            /// @return vector of user names

            std::vector<std::string>
            userName(const std::vector<int> &vi) const
            {
                std::vector<std::string> ret;
                for (int i : vi)
                    ret.push_back(userName(i));
                return ret;
            }

            /// @brief human readable graph deecription
            /// @return 
            std::string text();

        private:
            cAttribute myAtt;
        };
    }
}
