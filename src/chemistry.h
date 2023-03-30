#pragma once
#include "cGraph.h"

namespace raven
{
    namespace graph
    {
        /// @brief Convert chemical graph to SMILES representation
        /// @param g 
        /// @return SMILES string 
        std::string SMILES( const cGraph& g );

        /// @brief Convert SMILES string to chemical graph
        /// @param[out] g graph
        /// @param[in] smiles 
        void readSMILES( cGraph& g, const std::string& smiles);

        /// @brief Layout chemical graph
        /// @param g 
        std::string chemViz( const cGraph& g);
    }
}