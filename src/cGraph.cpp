#include <sstream>
#include "cGraph.h"
namespace raven
{
    namespace graph
    {
        std::string cGraph::text()
        {
            std::stringstream ss;
            for (int ei = 0; ei < edgeCount(); ei++)
            {
                int src = source(ei);
                int d = dest(ei);
                ss << ei 
                << " " << userName(src) << " ("<< src << ") "
                << userName(d) <<" ("<< d <<") "
                << rEdgeAttr(ei,0)
                << "\n";
            }
            return ss.str();
        }
    }
}