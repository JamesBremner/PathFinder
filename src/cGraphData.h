#pragma once
#include <string>
#include <vector>



/// @brief A graph of vertices connected by edges
class cGraphData
{
public:

    cGraphData();

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
    The vertex name must be the sting format index

    For large graphs this is much faster than adding edges by vertex names
    but the vertex names must be the vertex indices.
    
    */
    int add(
        int src,
        int dst,
        const std::string &sAttr = "1");

    /// @brief add edge, add required vertices
    /// @param srcName
    /// @param dstName
    /// @param sAttr single attribute value, string representation
    /// @return edge index
    int findorAdd(
        const std::string &srcName,
        const std::string &dstName,
        const std::string &sAttr);
    int findorAdd(
        int src,
        int dst,
        const std::string &sAttr);

    void remove(
        const std::string &srcName,
        const std::string &dstName);
    void remove(
        int src,
        int dst     );

    void edgeAttr( int ie, const std::vector<std::string>& vsAttr );
    void vertexAttr( int iv, const std::vector<std::string>& vsAttr );

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

    std::vector<std::string> userName(std::vector<int> vi);

    int find(int src, int dst) const;

    /// @brief vertices reachable with one hop
    /// @param vi starting vertex
    /// @return vector of reachable vertex indices

    std::vector<int> adjacentOut(int vi) const;

    std::vector<std::string> adjacentOut(const std::string& name ) const;

    double edgeAttr(int src, int dst, int ai) const;

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

    /* vertex indices of edge destinations

        vEdgeDst[ei] is the destination vertex index of the eith edge
    */
    std::vector<int> vEdgeDst;

    /** vertex attributes
     *
     *  vVertexAttr[vi][ai] is a string representing the aith attribute of the vith vertex
     */

    std::vector<std::vector<std::string>> vVertexAttr;

    /* edge attributes

        vEdgeAttr[ei][ai] is a string representing the aith attribute of the eith edge
    */
    std::vector<std::vector<std::string>> vEdgeAttr;
};