#pragma once
#include "cVE.h"

namespace raven
{
    namespace graph {

    /// @brief graph vertex and edge attributes

    class cAttribute 
    {
    public:

        void clear();

        /// @brief Change number of vertices
        /// @param c new vertex count

        void resizeVertex( int c )
        {
            vVertexName.resize(c);
            vVertexAttr.resize(c);
        }

        /// @brief add a vertex
        /// @param name name of new vertes

        void add( const std::string& name )
        {
            vVertexName.push_back( name );
        }

        /// @brief write vertex name
        /// @param iv 
        /// @param name 

        void wVertexName( int iv, const std::string& name )
        {
            vVertexName[iv] = name;
        }

        /// @brief write vertex attributes
        /// @param iv vertex index
        /// @param vsAttr vector of strings to replace vertex attributes
        void wVertexAttr(int iv, const std::vector<std::string> &vsAttr);

        /// @brief write edge attributes
        /// @param ei edge index
        /// @param vsAttr vector of strings to replace vertex attributes
        void wEdgeAttr(int ei, const std::vector<std::string> &vsAttr);

        void wEdgeAttr( int v1, int v2,  const std::vector<std::string> &vsAttr );

        /// @brief read vertex attributes
        /// @param vi vertex index
        /// @param ai attrinute index
        /// @return string representation of attribute
        std::string rVertexAttr(int vi, int ai ) const;

        /// @brief read edge attributes
        /// @param ei edge index
        /// @param ai attrinute index
        /// @return string representation of attribute, "" if edge or attribute doen't exist
        std::string rEdgeAttr(int ei, int ai ) const;

        /// @brief vertex index from name
        /// @param name 
        /// @return index

        int find( const std::string& name ) const;

        /// @brief  vertex name from index
        /// @param vi 
        /// @return vertex name

        std::string userName( int vi ) const
        {
            return vVertexName[vi];
        }



    private:
        /** vertex attributes
         *
         *  vVertexAttr[vi][ai] is a string representing the aith attribute of the vith vertex
         */

        std::vector<std::string> vVertexName;
        std::vector<std::vector<std::string>> vVertexAttr;

        /* edge attributes

            vEdgeAttr[ei][ai] is a string representing the aith attribute of the eith edge
        */
        std::vector<std::vector<std::string>> vEdgeAttr;
    };

}
}