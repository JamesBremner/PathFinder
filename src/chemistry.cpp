#include <sstream>
#include "GraphTheory.h"
#include "chemistry.h"
namespace raven
{
    namespace graph
    {
        std::string SMILES(
            const cGraph &ingraph,
            const std::vector<int> &bondtype)
        {
            std::string SMILES;              // the SMILES representation of the chemical graph
            std::vector<std::string> vLabel; // the label locations

            raven::graph::cGraph g = ingraph;

            // remove bonds to Hydrogen
            for (auto &bond : g.edgeList())
            {
                if (g.userName(bond.first)[0] == 'H' || g.userName(bond.second)[0] == 'H')
                    g.remove(bond.first, bond.second);
            }

            // check for cycles
            auto vCycle = dfs_cycle_finder(g);
            for (int k = 0; k < vCycle.size(); k++)
            {
                g.remove(vCycle[k][0], vCycle[k][1]);
                g.wVertexAttr(vCycle[k][0], {std::to_string(k + 1)});
                g.wVertexAttr(vCycle[k][1], {std::to_string(k + 1)});
            }

            // depth first search of graph
            int prev = -1;
            dfs(g, 0,
                [&](int v)
                {
                    // check for double bond
                    if (prev != -1)
                    {
                        if (bondtype[g.find(prev, v)] == 2)
                        {
                            SMILES += "=";
                            vLabel.push_back("-");
                        }
                    }

                    // get the atom label
                    auto label = g.userName(v);

                    switch (label[0])
                    {
                    case 'C':
                    case 'O':
                    case 'N':
                        vLabel.push_back(label);
                        SMILES += label[0];
                        break;
                    default:
                        vLabel.push_back("-");
                        break;
                    }

                    // check for cycle break
                    auto kc = g.rVertexAttr(v, 0);
                    if (!kc.empty())
                    {
                        SMILES += kc;
                        vLabel.push_back("-");
                    }

                    prev = v;

                    return true;
                });

            // std::cout << "SMILES: " << SMILES << "\n";
            // std::cout << "\nlabel locations in SMILES string:\n";
            // for (int k = 0; k < vLabel.size(); k++)
            //     std::cout << k << "\t" << vLabel[k] << "\n";

            return SMILES;
        }

        void readSMILES(
            cGraph &g,
            std::vector<int> &bondtype,
            const std::string &sin)
        {
            g.clear();
            char token;
            int branch;   // index of atom where branch occurs
            int src = -1; // index of atom waiting for bond
            std::vector<int> ring(10, -1);
            int ringID;
            int bond = 1;
            bool closebracket = false;
            bool square = false;
            int idx = 0;
            for (int p = 0; p < sin.length(); p++)
            {
                token = sin[p];
                // std::cout << token << " " << branch << " " << src << "\n";
                switch (token)
                {
                case 'C':
                case 'N':
                case 'O':
                    g.wVertexAttr(
                        g.add(std::to_string(idx)),
                        {std::string(1, token)});
                    idx++;
                    if (src == -1)
                    {
                        // root atom
                        src = idx - 1;
                        break;
                    }
                    if (closebracket)
                    {
                        int ie = g.add(
                            std::to_string(branch),
                            std::to_string(idx - 1));
                        if (ie > (int)bondtype.size() - 1)
                            bondtype.resize(ie + 1, -1);
                        bondtype[ie] = bond;
                        closebracket = false;
                    }
                    else
                    {
                        int ie = g.add(
                            std::to_string(src),
                            std::to_string(idx - 1));
                        if (ie > (int)bondtype.size() - 1)
                            bondtype.resize(ie + 1, -1);
                        bondtype[ie] = bond;
                    }
                    bond = 1;
                    src = idx - 1;
                    break;

                case '=':
                    bond = 2;
                    break;
                case 'H':
                    // Hydrogen is ignored
                    break;
                case '[':
                    square = true;
                    break;
                case ']':
                    square = false;
                    break;
                case '(':
                    branch = idx - 1;
                    break;
                case ')':
                    closebracket = true;
                    break;
                case '1':
                case '2':
                    if (square)
                        break;
                    ringID = (int)token - 48;
                    if (ring[ringID] == -1)
                    {
                        ring[ringID] = idx - 1;
                    }
                    else
                    {
                        g.add(std::to_string(ring[ringID]), std::to_string(src));
                        ring[ringID] = -1;
                    }
                    break;
                }
            }
        }

        std::string chemViz(
            const cGraph &g,
            const std::vector<int>& bondtype)
        {
            std::map<std::string, std::string> mpColor;
            mpColor.insert({"C", "grey"});
            mpColor.insert({"O", "red"});
            mpColor.insert({"N", "green"});

            std::stringstream sviz;
            sviz << "graph G {\n";
            for (int ni = 0; ni < g.vertexCount(); ni++)
            {
                auto atom = g.rVertexAttr(ni, 0);
                sviz << ni
                     << " [label=\"" << atom
                     << "\" color = \"" << mpColor[atom]
                     << "\"  penwidth = 3.0 ];\n";
            }
            for (auto &e : g.edgeList())
            {
                std::string color = "";
                if (bondtype[g.find(e.first, e.second)] == 2)
                    color = " [penwidth = 3.0]";
                sviz << e.first << "--"
                     << e.second
                     << color
                     << "\n";
            }
            sviz << "}\n";

            return sviz.str();
        }
    }
}
