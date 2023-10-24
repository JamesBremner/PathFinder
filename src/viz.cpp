#include <sstream>
#include <fstream>
#include <filesystem>
#include "cGraph.h"
#include <wex.h>
#include "viz.h"

namespace raven
{
    namespace graph
    {
        cViz::cViz()
        {
            fnVertexColor = [](int v)
            { return ""; };
        }
        void cViz::setVertexColor(std::function<std::string(int)> f)
        {
            fnVertexColor = f;
        }
        std::string cViz::makeDot(const cGraph &g)
        {
            std::string graphvizgraph = "graph";
            std::string graphvizlink = "--";

            std::stringstream ss;
            ss << graphvizgraph << " G {\n";

            // loop over vertices
            for (int vi = 0; vi < g.vertexCount(); vi++)
            {
                ss << "\"" << g.userName(vi) << "\""
                   << " [  penwidth = 3.0"
                   << fnVertexColor(vi) << " ];\n";
            }

            // loop over edges
            for (int v1 = 0; v1 < g.vertexCount(); v1++)
            {
                for (int v2 : g.adjacentOut(v1))
                {
                    // check not reverse link in undirected graph
                    if (v2 > v1)
                        continue;

                    ss << "\"" << g.userName(v1) << "\"" << graphvizlink
                       << "\"" << g.userName(v2) << "\";\n";
                }
            }
            ss << "}\n";

            return ss.str();
        }
        void cViz::viz(
            const cGraph &g)
        {
            // open file to store the VizGraph DOT representation of the graph
            // https://graphviz.org/doc/info/lang.html

            // temp directory is usually
            // C:\Users\<userName>\AppData\Local\Temp
            auto path = std::filesystem::temp_directory_path();
            // std::cout << "RunDOT " << path << "\n";
            auto gdot = path / "g.dot";
            std::ofstream f(gdot);
            if (!f.is_open())
                throw std::runtime_error("Cannot open " + gdot.string());

            // write dot representation to file
            f << makeDot(g);
            f.close();

            // run Graphviz dot program to layout the graph
            // and create a PNG image file

            auto sample = path / "sample.png";
            std::string scmd = "dot -Kfdp -n -Tpng -Tdot -o " + sample.string() + " " + gdot.string();
            std::string error;
            if (wex::free::startProcess(
                    scmd,
                    error))
                throw std::runtime_error(
                    "GraphViz dot failed " + error);
        }

    }
}