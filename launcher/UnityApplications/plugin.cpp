#include "launcherapplication.h"
#include "place.h"
#include "placeentry.h"
#include "launcherdevice.h"
#include "trash.h"
#include "listaggregatormodel.h"
#include "launcherapplicationslist.h"
#include "launcherdeviceslist.h"
#include "launcherplaceslist.h"
#include "iconimageprovider.h"
#include "x11windowimageprovider.h"
#include "windowinfo.h"
#include "windowslist.h"
#include "plugin.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QAbstractListModel>

void UnityApplicationsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<ListAggregatorModel>(uri, 0, 1, "ListAggregatorModel");

    qmlRegisterType<LauncherApplicationsList>(uri, 0, 1, "LauncherApplicationsList");
    qmlRegisterType<LauncherApplication>(uri, 0, 1, "LauncherApplication");

    qmlRegisterType<LauncherPlacesList>(uri, 0, 1, "LauncherPlacesList");
    qmlRegisterType<Place>(uri, 0, 1, "Place");
    qmlRegisterType<PlaceEntry>(uri, 0, 1, "PlaceEntry");

    qmlRegisterType<LauncherDevicesList>(uri, 0, 1, "LauncherDevicesList");
    qmlRegisterType<LauncherDevice>(uri, 0, 1, "LauncherDevice");

    qmlRegisterType<Trashes>(uri, 0, 1, "Trashes");
    qmlRegisterType<Trash>(uri, 0, 1, "Trash");

    qmlRegisterType<WindowInfo>(uri, 0, 1, "WindowInfo");
    qmlRegisterType<WindowsList>(uri, 0, 1, "WindowsList");
}

void UnityApplicationsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImageProvider(QString("icons"), new IconImageProvider);
    engine->addImageProvider(QString("x11"), new X11WindowImageProvider);
}

Q_EXPORT_PLUGIN2(UnityApplications, UnityApplicationsPlugin);
