#include <stdexcept>
#include "cAttribute.h"
namespace raven
{ namespace graph {
    void cAttribute::clear()
    {
        vVertexAttr.clear();
        vEdgeAttr.clear();
    }

    void cAttribute::wVertexAttr(int iv, const std::vector<std::string> &vsAttr)
    {
        if (0 > iv)
            throw std::runtime_error(
                "cGraphData::wVertexAttr bad index");
        if (iv > (int)vVertexAttr.size() - 1)
            vVertexAttr.resize(iv + 1);
        vVertexAttr[iv] = vsAttr;
    }
    std::string cAttribute::rVertexAttr(int vi, int ai) const
    {
        if (0 > vi || vi > (int)vVertexAttr.size() - 1)
            return "";
        if (0 > ai || ai > (int)vVertexAttr[vi].size() - 1)
            return "";
        return vVertexAttr[vi][ai];
    }
    void cAttribute::wEdgeAttr(int ie, const std::vector<std::string> &vsAttr)
    {
        if (0 > ie)
            throw std::runtime_error(
                "cGraphData::wEdgeAttr bad index");
        if (ie > (int)vEdgeAttr.size() - 1)
            vEdgeAttr.resize(ie + 1);
        vEdgeAttr[ie] = vsAttr;
    }


    std::string cAttribute::rEdgeAttr(int ei, int ai) const
    {
        if (0 > ei || ei > (int)vEdgeAttr.size() - 1)
            return "";
        if (0 > ai || ai > (int)vEdgeAttr[ei].size() - 1)
            return "";
        return vEdgeAttr[ei][ai];
    }
}
}