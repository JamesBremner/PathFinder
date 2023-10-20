#include <functional> 

namespace raven
{
    namespace graph
    {
        /// Layout and generate image of graph using GraphViz
        class cViz
        {
            std::string makeDot(const cGraph &g);
            std::function<std::string(int)> fnVertexColor;

        ////////////////////////////////////////    
        public:

            cViz();
            /// @brief  Register function to set color of vertex
            /// @param f  function 
            ///
            /// The function must have the signature std::string f( int v )
            /// v the vertex index
            /// return string in format ", color = red"

            void setVertexColor( std::function<std::string(int)> f);

            /// Layout and create image of graph
            /// @param g the graph

            void viz(const cGraph &g);
        };
    }
}