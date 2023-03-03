#pragma once
#include "cVE.h"
#include "cAttribute.h"

namespace raven
{
    namespace graph
    {

        class cGraph : public cVE
        {
        public:
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
