#include <string>
#include <vector>

class cGraphData
{
    public:

    /// @brief add vertex
    /// @param vertexName 
    /// @return vertex index
    int add( const std::string& vertexName );

    /// @brief add edge
    /// @param srcName 
    /// @param dstName 
    /// @return edge inex
    int add( const std::string& srcName, const std::string& dstName );

    /// @brief find vertex with name
    /// @param vertexName 
    /// @return vertex index, -1 if not found
    int find( const std::string& vertexName ) const;

    /// @brief human readable edge list
    /// @return text
    std::string text() const;

    private:

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