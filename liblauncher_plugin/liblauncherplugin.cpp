#include "launcherapplication.h"
#include "liblauncherplugin.h"
#include <launcher/launcher.h>
#include <QtDeclarative/qdeclarative.h>

void LibLauncherPlugin::registerTypes(const char *uri)
{
    /* Force the creation of a LauncherSession object so that it starts
       listening to window events from wnck and appropriately updates
       LauncherApplication objects upon launch of applications. */
    LauncherSession *launcher_session;
    launcher_session = launcher_session_get_default();

    qmlRegisterType<QLauncherApplication>(uri, 0, 1, "QLauncherApplication");
}

Q_EXPORT_PLUGIN2(liblauncherplugin, LibLauncherPlugin);
