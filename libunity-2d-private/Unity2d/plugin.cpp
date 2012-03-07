/*
 * Copyright (C) 2010-2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Required otherwise using wnck_set_client_type breaks linking with error:
   undefined reference to `wnck_set_client_type(WnckClientType)'
*/
#include "plugin.h"

#include "dashclient.h"
#include "hudclient.h"
#include "application.h"
#include "launcherdevice.h"
#include "trash.h"
#include "workspaces.h"
#include "listaggregatormodel.h"
#include "applicationslist.h"
#include "launcherdeviceslist.h"
#include "iconimageprovider.h"
#include "blendedimageprovider.h"
#include "qsortfilterproxymodelqml.h"
#include "windowimageprovider.h"
#include "windowinfo.h"
#include "windowslist.h"
#include "screeninfo.h"
#include "desktopinfo.h"
#include "plugin.h"
#include "cacheeffect.h"
#include "iconutilities.h"
#include "cursorshapearea.h"
#include "unity2dtr.h"
#include "giodefaultapplication.h"

#include "mimedata.h"
#include "dragdropevent.h"
#include "dragitemwithurl.h"
#include "dropitem.h"
#include "launcherdropitem.h"

#include "config.h"

#include "bfb.h"
#include "gesturehandler.h"

#include "lenses.h"
#include "lens.h"

#include "hud.h"

#include "percentcoder.h"

#include "filter.h"
#include "filters.h"
#include "ratingsfilter.h"
#include "radiooptionfilter.h"
#include "checkoptionfilter.h"
#include "multirangefilter.h"
#include "windowsintersectmonitor.h"
#include "spreadmonitor.h"
#include "focuspath.h"

#include "unity2ddeclarativeview.h"
#include "inputshapemanager.h"
#include "inputshaperectangle.h"
#include "inputshapemask.h"
#include "unity2dpanel.h"
#include "strutmanager.h"

#include "pointerbarrier.h"

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QGraphicsEffect>
#include <QAbstractListModel>
#include <QTextCodec>

// QtDee
#include "deelistmodel.h"

#include <X11/Xlib.h>

/* FIXME: This should be done more properly, it's just an hack this way.
          We should silence only the errors that we know we can produce.
          We could probably also learn something from gdk-error-trap-push.
          See: http://library.gnome.org/devel/gdk/stable/gdk-General.html#gdk-error-trap-push
 */
static int _x_errhandler(Display* display, XErrorEvent* event)
{
    Q_UNUSED(display);
    Q_UNUSED(event);

    return 0;
 }

void Unity2dPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 0, 1, "SortFilterProxyModel");

    qmlRegisterType<DashClient>();
    qmlRegisterType<HUDClient>();

    qmlRegisterType<WindowInfo>(uri, 0, 1, "WindowInfo");
    qmlRegisterType<WindowsList>(uri, 0, 1, "WindowsList");
    qmlRegisterType<ScreenInfo>(uri, 0, 1, "ScreenInfo");
    qmlRegisterType<DesktopInfo>(); // Register the type as non creatable
    qmlRegisterType<WorkspacesInfo>(); // Register the type as non creatable

    qmlRegisterType<CacheEffect>(uri, 0, 1, "CacheEffect");
    qmlRegisterType<QGraphicsBlurEffect>("Effects", 1, 0, "Blur");
    qmlRegisterType<QGraphicsColorizeEffect>("Effects", 1, 0, "Colorize");
    qmlRegisterType<QGraphicsDropShadowEffect>("Effects", 1, 0, "DropShadow");

    /* Custom drag’n’drop implementation in QML */
    qmlRegisterType<DeclarativeDragItemWithUrl>(uri, 0, 1, "DragItemWithUrl");
    qmlRegisterType<DeclarativeDropItem>(uri, 0, 1, "DropItem");
    qmlRegisterType<DeclarativeMimeData>();
    qmlRegisterType<DeclarativeDragDropEvent>();
    qmlRegisterType<LauncherDropItem>(uri, 0, 1, "LauncherDropItem");

    qmlRegisterType<ListAggregatorModel>(uri, 0, 1, "ListAggregatorModel");

    qmlRegisterType<BfbModel>(uri, 0, 1, "BfbModel");
    qmlRegisterType<BfbItem>(uri, 0, 1, "BfbItem");

    qmlRegisterType<ApplicationsList>(uri, 0, 1, "ApplicationsList");
    qmlRegisterType<Application>(uri, 0, 1, "Application");

    qmlRegisterType<DeeListModel>(uri, 0, 1, "DeeListModel");

    qmlRegisterType<LauncherDevicesList>(uri, 0, 1, "LauncherDevicesList");
    qmlRegisterType<LauncherDevice>(uri, 0, 1, "LauncherDevice");

    qmlRegisterType<Trashes>(uri, 0, 1, "Trashes");
    qmlRegisterType<Trash>(uri, 0, 1, "Trash");

    qmlRegisterType<WorkspacesList>(uri, 0, 1, "WorkspacesList");
    qmlRegisterType<Workspaces>(uri, 0, 1, "Workspaces");

    qmlRegisterType<IconUtilities>(); // Register the type as non creatable

    qmlRegisterType<CursorShapeArea>(uri, 0, 1, "CursorShapeArea");

    qmlRegisterType<GioDefaultApplication>(uri, 0, 1, "GioDefaultApplication");

    qmlRegisterType<Lenses>(uri, 1, 0, "Lenses");
    qmlRegisterType<Lens>(uri, 1, 0, "Lens");

    qmlRegisterType<Hud>(uri, 1, 0, "Hud");

    qmlRegisterType<PercentCoder>(uri, 0, 1, "PercentCoder");

    qmlRegisterType<FocusPath>(uri, 0, 1, "FocusPath");

    qmlRegisterType<Filter>();
    qmlRegisterType<Filters>();
    qmlRegisterType<RatingsFilter>();
    qmlRegisterType<RadioOptionFilter>();
    qmlRegisterType<CheckOptionFilter>();
    qmlRegisterType<MultiRangeFilter>();
    qmlRegisterType<FilterOption>();
    qmlRegisterType<FilterOptions>();

    qmlRegisterType<GestureHandler>(uri, 0, 1, "GestureHandler");
    qmlRegisterType<WindowsIntersectMonitor>(uri, 0, 1, "WindowsIntersectMonitor");
    qmlRegisterType<SpreadMonitor>(uri, 0, 1, "SpreadMonitor");

    qmlRegisterType<InputShapeManager>(uri, 0, 1, "InputShapeManager");
    qmlRegisterType<InputShapeRectangle>(uri, 0, 1, "InputShapeRectangle");
    qmlRegisterType<InputShapeMask>(uri, 0, 1, "InputShapeMask");
    qmlRegisterType<Unity2DDeclarativeView>();

    qmlRegisterType<Unity2dPanel>(uri, 0, 1, "Unity2dPanel");
    qmlRegisterType<StrutManager>(uri, 0, 1, "StrutManager");
    
    qmlRegisterType<PointerBarrierWrapper>(uri, 0, 1, "PointerBarrier");
}

void Unity2dPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    // XSetErrorHandler(_x_errhandler);

    engine->addImageProvider(QString("blended"), new BlendedImageProvider(engine->baseUrl()));
    engine->addImageProvider(QString("window"), new WindowImageProvider);
    engine->addImageProvider(QString("icons"), new IconImageProvider);

    /* ScreenInfo is exposed as a context property as it's a singleton and therefore
       not creatable directly in QML */
    engine->rootContext()->setContextProperty("desktop", DesktopInfo::instance());
    engine->rootContext()->setContextProperty("iconUtilities", new IconUtilities(engine));

    /* Expose QConf objects as a context property not to initialize it multiple times */
    engine->rootContext()->setContextProperty("unity2dConfiguration", &unity2dConfiguration());
    engine->rootContext()->setContextProperty("launcher2dConfiguration", &launcher2dConfiguration());
    engine->rootContext()->setContextProperty("dash2dConfiguration", &dash2dConfiguration());

    /* Critically important to set the client type to pager because wnck
       will pass that type over to the window manager through XEvents.
       Window managers tend to respect orders from pagers to the letter by
       for example bypassing focus stealing prevention.
       Compiz does exactly that in src/event.c:handleEvent(...) in the
       ClientMessage case (line 1702).
       Metacity has a similar policy in src/core/window.c:window_activate(...)
       (line 2951).
    */
    wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");
    Unity2dTr::qmlInit(engine->rootContext());

    /* Define the charset that Qt assumes C-strings (char *) and std::string to be in.
       After that definition, using QString::fromStdString and QString::toStdString
       will properly convert from and to std::string encoded in UTF-8 as it is
       the case in Unity's shared backend.

       Ref.: http://developer.qt.nokia.com/wiki/QtStrings
    */
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
}

Q_EXPORT_PLUGIN2(Unity2d, Unity2dPlugin);
