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
    /// @return vertex index
    int add();

    void add( int i );


    /// @brief add edge using vertex indices
    /// @param src source index
    /// @param dst destination index
    /// @return edge index
    /*  
    The vertices need not exist ( if not they will be added )

    */
    int add(
        int src,
        int dst);


    int findorAdd(
        int src,
        int dst);


    // void remove(
    //     int src,
    //     int dst     );

    /// @brief remove edge
    /// @param ei edge index
    void remove(
        int ei );



    //////////////////////// GETTERS

    bool isDirected() const
    {
        return fDirected;
    }
    int vertexCount() const
    {
        return myVertexCount;
    }

    int edgeCount() const;

    /// @brief find vertex with name
    /// @param vertexName
    /// @return vertex index, -1 if not found

    int find(const std::string &vertexName) const;

    /// @brief find edge
    /// @param src 
    /// @param dst 
    /// @return edge index, -1 if no edge found
    
    int find(int src, int dst) const;

    /// @brief vertices reachable with one hop
    /// @param vi starting vertex
    /// @return vector of reachable vertex indices

    std::vector<int> adjacentOut(int vi) const;

    //std::vector<std::string> adjacentOut(const std::string& name ) const;

    /// @brief vertices that can reach specified vertex in one hop
    /// @param vi 
    /// @return 
    std::vector<int> adjacentIn(int vi) const;

    int source( int ei ) const;
    int dest( int ei ) const;

    /// @brief human readable edge list
    /// @return text

    std::string text() const;

    std::vector<std::pair<int,int>>
    edgeList() const;

private:

    bool fDirected;

    int myVertexCount;

    /* vertex user names

        vVertex[ vi ] is the name of the vith zero-based vertex
    */
    //std::vector<std::string> vVertexName;

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
