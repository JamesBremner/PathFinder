#pragma once
#include <string>
#include <vector>

namespace raven { namespace graph {

/// @brief A graph of vertices connected by edges
class cVE
{
public:

    cVE();

    void clear();

    void directed( bool f = true );

    /// @brief add vertex
    /// @param vertexName
    /// @return vertex index
    int add(const std::string &vertexName);

    int findorAdd(const std::string vertexName);

    /// @brief add edge using vertex user names, vertices must exist
    /// @param srcName
    /// @param dstName
    /// @return edge index ( throws exception on failure )
    int add(
        const std::string &srcName,
        const std::string &dstName);

    /// @brief add edge using vertex indices
    /// @param src source index
    /// @param dst destination index
    /// @param sAttr edge attribite
    /// @return edge index
    /*  
    The vertices need not exist ( if not they will be added )

    For large graphs this is much faster than adding edges by vertex names
    but the vertex names must be the vertex indices.

    */
    int add(
        int src,
        int dst);

    /// @brief add edge, add required vertices
    /// @param srcName
    /// @param dstName
    /// @return edge index
    int findorAdd(
        const std::string &srcName,
        const std::string &dstName);
    int findorAdd(
        int src,
        int dst);

    // void remove(
    //     const std::string &srcName,
    //     const std::string &dstName);
    void remove(
        int src,
        int dst     );
    void remove(
        int ei );



    //////////////////////// GETTERS

    bool isDirected() const
    {
        return fDirected;
    }
    int vertexCount() const
    {
        return vVertexName.size();
    }

    int edgeCount() const;

    /// @brief find vertex with name
    /// @param vertexName
    /// @return vertex index, -1 if not found

    int find(const std::string &vertexName) const;

    /// @brief vertex name from index
    /// @param i vertex index
    /// @return vertex name

    std::string userName(int i) const;

    /// @brief vertex names from vertex indices
    /// @param vi vector of indices
    /// @return vector of names

    std::vector<std::string> userName(std::vector<int> vi) const;

    std::vector<std::string> vertexNames() const;

    /// @brief find edge
    /// @param src 
    /// @param dst 
    /// @return edge index, -1 if no edge found
    
    int find(int src, int dst) const;

    /// @brief vertices reachable with one hop
    /// @param vi starting vertex
    /// @return vector of reachable vertex indices

    std::vector<int> adjacentOut(int vi) const;

    std::vector<std::string> adjacentOut(const std::string& name ) const;

    /// @brief vertices that can reach specified vertex in one hop
    /// @param vi 
    /// @return 
    std::vector<int> adjacentIn(int vi) const;

    /// @brief human readable edge list
    /// @return text

    std::string text() const;

    std::vector<std::pair<int,int>>
    edgeList() const;

private:

    bool fDirected;

    /* vertex user names

        vVertex[ vi ] is the name of the vith zero-based vertex
    */
    std::vector<std::string> vVertexName;

    /* edge indices that start at each vertex

        vOutEdges[vi][wi] is the index of the with edge starting at the vith vertex
    */
    std::vector<std::vector<int>> vOutEdges;

    std::vector<std::vector<int>> vInEdges;

    /* vertex indices of edge destinations

        vEdgeDst[ei] is the destination vertex index of the eith edge
    */
    std::vector<int> vEdgeDst;

};

}
}
