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
                myAtt.wEdgeAttr(find(v1, v2), vsAttr);
                if (!isDirected())
                    myAtt.wEdgeAttr(find(v2, v1), vsAttr);
            }


            // readers
            
            std::string rVertexAttr( int vi, int ai ) const
            {
                return myAtt.rVertexAttr( vi, ai );
            }
            std::string rEdgeAttr( int ei, int ai ) const
            {
                return myAtt.rEdgeAttr( ei, ai );
            }

        private:
            cAttribute myAtt;
        };

    }
}
