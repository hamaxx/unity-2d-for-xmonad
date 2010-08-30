#include "launcherapplication.h"
#include "iconimageprovider.h"
#include "launcherplugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>

void LauncherPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QLauncherApplication>(uri, 0, 1, "QLauncherApplication");
}

void LauncherPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    engine->addImageProvider(QString("icons"), new IconImageProvider);
}

Q_EXPORT_PLUGIN2(launcherplugin, LauncherPlugin);
