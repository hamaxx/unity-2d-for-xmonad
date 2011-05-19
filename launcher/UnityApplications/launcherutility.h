
#ifndef LAUNCHERUTILITY_H
#define LAUNCHERUTILITY_H

#include <gio/gdesktopappinfo.h>
#include <libwnck/libwnck.h>

class LauncherUtility {

public:
    static void showWindow(WnckWindow* window);
    static void moveViewportToWindow(WnckWindow* window);
};

#endif // LAUNCHERUTILITY_H
