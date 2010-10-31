#include "launcherapplication.h"
#include "device.h"
#include "launchermodel.h"
#include "iconimageprovider.h"
#include "launchermenu.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

void UnityApplicationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<LauncherApplication>(uri, 0, 1, "LauncherApplication");
    qmlRegisterType<Device>(uri, 0, 1, "Device");
}

void UnityApplicationsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("icons"), new IconImageProvider);

    LauncherModel* launcher = new LauncherModel;
    engine->rootContext()->setContextProperty("launcher", launcher);

    QLauncherContextualMenu* menu = new QLauncherContextualMenu;
    engine->rootContext()->setContextProperty("menu", menu);
}

Q_EXPORT_PLUGIN2(UnityApplications, UnityApplicationsPlugin);
