#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "iconimageprovider.h"
#include "launcherplugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

void LauncherPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QLauncherApplication>(uri, 0, 1, "QLauncherApplication");
}

void LauncherPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    engine->addImageProvider(QString("icons"), new IconImageProvider);

    LauncherApplicationsList* applications = new LauncherApplicationsList;
    engine->rootContext()->setContextProperty("launcher_applications", applications);
}

Q_EXPORT_PLUGIN2(launcherplugin, LauncherPlugin);
