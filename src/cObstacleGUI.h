#include <string>
#include <vector>
#include <wex.h>
#include "cObstacle.h"

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
    cObstacle myObstacle;
    enum class eView
    {
        route,
        routelist,
        span,
        input,
        calculating,
    };
    eView myViewType;
    std::string myfname;

    void ConstructMenu();

    void draw(PAINTSTRUCT &ps);

    void drawInput(wex::shapes &S);
    void drawObstacles(
        wex::shapes &S,
        int scale);
    void drawRouteList(wex::shapes &S);
    void drawTour(wex::shapes& S, int scale );
    void drawSpanningTree(wex::shapes &S, int scale );
};
