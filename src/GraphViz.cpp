
#include <sstream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include "cGraphData.h"

namespace raven
{

    std::string pathViz(
        cGraphData &g,
        std::vector<int> &vpath,
        bool all)
    {
        std::string graphvizgraph = "graph";
        std::string graphvizlink = "--";
        if (g.isDirected())
        {
            graphvizgraph = "digraph";
            graphvizlink = "->";
        }

        std::stringstream f;
        f << graphvizgraph << " G {\n";

        // loop over vertices
        for (auto &n : g.vertexNames())
        {
            f << n
              << " [  penwidth = 3.0 ];\n";
        }

        // std::cout << "pathViz " << pathText() << "\n";

        // loop over links
        for (int v1 = 0; v1 < g.vertexCount(); v1++)
        {
            for (int v2 : g.adjacentOut(v1))
            {
                // check not reverse link in undirected graph
                if (!g.isDirected())
                    if (v2 > v1)
                        continue;

                f << g.userName(v1) << graphvizlink
                  << g.userName(v2);

                // check for link on path
                for (int kp = 1; kp < vpath.size(); kp++)
                {
                    if (v1 == vpath[kp - 1] &&
                        v2 == vpath[kp])
                    {
                        f << "[color=\"red\"] ";
                        break;
                    }
                    if (!g.isDirected())
                    {
                        if (v2 == vpath[kp - 1] &&
                            v1 == vpath[kp])
                        {
                            f << "[color=\"red\"] ";
                            break;
                        }
                    }
                }

                f << ";\n";
            }
        }

        // for (link_t e : links())
        // {
        //     if (!isDirected())

        //         /* an undirected graph has a pair of directed links
        //         beteen every pair of linked nodes. Suppress the display
        //         of the second link of the pair
        //         */
        //         if (e.first.first > e.first.second)
        //             continue;

        //     // check if link between two nodes on path
        //     bool onpath = isLinkOnPath(e);

        //     if (all)
        //     {
        //         f << node(e.first.first).myName << graphvizlink
        //           << node(e.first.second).myName;
        //         if (onpath)
        //             f << "[color=\"red\"] ";
        //     }
        //     else if (onpath)
        //     {
        //         f << node(e.first.first).myName << graphvizlink
        //           << node(e.first.second).myName
        //           << "[color=\"red\"] ";
        //     }
        //     f << "\n";
        // }

        f << "}\n";
        return f.str();
    }

    void RunDOT(cGraphData &g,
                const std::string &pathViz)
    {

        if (g.vertexCount() > 200)
            return;

        // temp firectory is usually
        // C:\Users\<userName>\AppData\Local\Temp

        auto path = std::filesystem::temp_directory_path();
        // std::cout << "RunDOT " << path << "\n";
        auto gdot = path / "g.dot";
        std::ofstream f(gdot);
        if (!f.is_open())
            throw std::runtime_error("Cannot open " + gdot.string());

        f << pathViz;

        // using raven::graph::eCalculation;
        // switch (opt)
        // {
        // case eCalculation::costs:
        // case eCalculation::sales:
        // case eCalculation::bonesi:
        // case eCalculation::srcnuzn:
        //     f << finder.pathViz() << "\n";
        //     break;
        // case eCalculation::spans:
        //     f << finder.spanViz() << "\n";
        //     break;
        // case eCalculation::cams:
        // case eCalculation::reqs:
        //     f << finder.camsViz() << "\n";
        //     break;
        // case eCalculation::pickup:
        //     f << finder.pickupViz() << "\n";
        //     break;
        // default:
        //     std::cout << "RunDOT bad option\n";
        //     return;
        // }

        f.close();

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        auto sample = path / "sample.png";
        std::string scmd = "dot -Kfdp -n -Tpng -Tdot -o " + sample.string() + " " + gdot.string();

        // std::cout << scmd << "\n";

        // Retain keyboard focus, minimize module2 window
        si.wShowWindow = SW_SHOWNOACTIVATE | SW_MINIMIZE;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION;
        si.dwX = 600;
        si.dwY = 200;

        if (!CreateProcess(NULL,                 // No module name (use command line)
                           (char *)scmd.c_str(), // Command line
                           NULL,                 // Process handle not inheritable
                           NULL,                 // Thread handle not inheritable
                           FALSE,                // Set handle inheritance to FALSE
                           CREATE_NEW_CONSOLE,   //  creation flags
                           NULL,                 // Use parent's environment block
                           NULL,                 // Use parent's starting directory
                           &si,                  // Pointer to STARTUPINFO structure
                           &pi)                  // Pointer to PROCESS_INFORMATION structure
        )
        {
            int syserrno = GetLastError();
            if (syserrno == 2)
                throw std::runtime_error(
                    "Cannot find executable file\n"
                    "Is graphViz installed?");

            throw std::runtime_error("RunDOT system error");
            // SetStatusText(wxString::Format("Sysem error no (%d)\n", GetLastError()));
            // wchar_t *lpMsgBuf;
            // FormatMessage(
            //     FORMAT_MESSAGE_ALLOCATE_BUFFER |
            //         FORMAT_MESSAGE_FROM_SYSTEM |
            //         FORMAT_MESSAGE_IGNORE_INSERTS,
            //     NULL,
            //     (DWORD)syserrno,
            //     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            //     (LPWSTR)&lpMsgBuf,
            //     0, NULL);

            // char errorbuf[200];
            // snprintf(errorbuf, 199,
            //          "Error is %S",
            //          lpMsgBuf);
            // LocalFree(lpMsgBuf);

            return;
        }

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        Sleep(4000);
    }
}
