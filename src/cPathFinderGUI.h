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



class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::panel &myplText;               // display text
    wex::panel &myplLayout;             // display layout ( graphViz )

    raven::graph::sGraphData myGraphData;       // the input graph
    raven::graph::cGraph myResultGraph; // the result graph

    raven::graph::cTourNodes *mypTourNodes;
    std::string myResultText;           // calculation result text

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
    void calcEuler();
    void calcCover();
    void calcExplore();

    void draw(PAINTSTRUCT &ps);
    void drawLayout(PAINTSTRUCT &ps);

    void drawInput(wex::shapes &S);
    void drawSpan(wex::shapes &S);
    void drawObstacles(
        wex::shapes &S,
        int scale);
};

extern cGUI theGUI;
