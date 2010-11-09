#include "launcherapplication.h"
#include "place.h"
#include "placeentry.h"
#include "launcherdevice.h"
#include "trash.h"
#include "launchermodel.h"
#include "iconimageprovider.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

void UnityApplicationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<LauncherApplication>(uri, 0, 1, "LauncherApplication");
    qmlRegisterType<Place>(uri, 0, 1, "Place");
    qmlRegisterType<PlaceEntry>(uri, 0, 1, "PlaceEntry");
    qmlRegisterType<LauncherDevice>(uri, 0, 1, "LauncherDevice");
    qmlRegisterType<Trash>(uri, 0, 1, "Trash");
}

void UnityApplicationsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("icons"), new IconImageProvider);

    LauncherModel* launcher = new LauncherModel;
    engine->rootContext()->setContextProperty("launcher", launcher);
}

Q_EXPORT_PLUGIN2(UnityApplications, UnityApplicationsPlugin);
