#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "iconimageprovider.h"
#include "launchermenu.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

void UnityApplicationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QLauncherApplication>(uri, 0, 1, "QLauncherApplication");
}

void UnityApplicationsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("icons"), new IconImageProvider);

    LauncherApplicationsList* applications = new LauncherApplicationsList;
    engine->rootContext()->setContextProperty("applications", applications);

    QLauncherContextualMenu* menu = new QLauncherContextualMenu;
    engine->rootContext()->setContextProperty("menu", menu);
}

Q_EXPORT_PLUGIN2(UnityApplications, UnityApplicationsPlugin);
