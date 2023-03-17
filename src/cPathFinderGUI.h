#include "wex.h"
#include "GraphTheory.h"

class cStarterGUI
{
public:
    /** CTOR
     * @param[in] title will appear in application window title
     * @param[in] vlocation set location and size of appplication window
     *
     * Usage:
     *
     * <pre>
class appGUI : public cStarterGUI
{
public:
    appGUI()
        : cStarterGUI(
              "The Appliccation",
              {50, 50, 1000, 500})
    {

        // initialize the solution
        ...

        show();
        run();
    }
    </pre>
    */
    cStarterGUI(
        const std::string &title,
        const std::vector<int> &vlocation)
        : fm(wex::maker::make())
    {
        fm.move(vlocation);
        fm.text(title);

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });
    }
    /** Draw nothing
     *
     * An application should over-ride this method
     * to perform any drawing reuired
     */
    virtual void draw(wex::shapes &S)
    {
    }
    void show()
    {
        fm.show();
    }
    void run()
    {
        fm.run();
    }

protected:
    wex::gui &fm;
};

enum class graph_calc
{
    none,
    cost,
    cycle,
    astar,
    tour,
    obs,
    spans,
    sales,
    cliques,
    flows,
    multiflows,
    allpaths,
    probs,
    alloc,
};

class cGUI : public cStarterGUI
{
public:
    cGUI();

    void start(const std::string &name)
    {
        myStartName.push_back( name );
    }
    void end(const std::string &name)
    {
        myEndName = name;
    }

private:
    wex::panel &myplText;
    wex::panel &myplLayout;
    
    raven::graph::cGraph myGraph;              // the input graph
    raven::graph::cGraph myResultGraph;              // the result graph

    raven::graph::cTourNodes * mypTourNodes;
    std::string myfname;
    graph_calc myCalcOption;
    std::vector<std::string> myStartName;
    std::string myEndName;
    std::string myResultText;

    enum class eView
    {
        route,
        span,
        input,
    };
    eView myViewType;

    void ConstructMenu();

    void calculate();
    void calcCost();
    void calcCycle();
    void calcSpan();
    void calcSales();
    void calcTour();
    void calcCliques();
    void calcFlows();
    void calcMultiFlows();
    void calcAllPaths();
    void calcProbs();
    void calcAlloc();

    void draw(PAINTSTRUCT &ps);
    void drawLayout(PAINTSTRUCT &ps);

    void drawInput(wex::shapes &S);
    void drawSpan(wex::shapes &S);
    void drawObstacles(
        wex::shapes &S,
        int scale);
};

extern cGUI theGUI;

/// @brief read input file
/// @param g graph to store input
/// @param fname path to input file
/// @return calculation option

graph_calc readfile(
    raven::graph::cGraph &g,
    const std::string &fname);
