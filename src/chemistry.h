#pragma once
#include "cGraph.h"

namespace raven
{
    namespace graph
    {
        /// @brief Convert chemical graph to SMILES representation
        /// @param g 
        /// @return SMILES string 
        std::string SMILES( 
            const cGraph& g,
            const std::vector<int>& bondtype );

        /// @brief Convert SMILES string to chemical graph
        /// @param[out] g graph
        /// @param[in] smiles 
        void readSMILES( cGraph& g,
        std::vector<int>& bondtype,
         const std::string& smiles);

        /// @brief Layout chemical graph
        /// @param g 
        std::string chemViz(
             const cGraph& g,
             const std::vector<int>& bondtype);
    }
}