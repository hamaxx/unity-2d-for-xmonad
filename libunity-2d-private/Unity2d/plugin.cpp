/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "blendedimageprovider.h"
#include "cacheeffect.h"
#include "qsortfilterproxymodelqml.h"
#include "windowimageprovider.h"
#include "windowinfo.h"
#include "windowslist.h"
#include "screeninfo.h"
#include "plugin.h"
#include "cacheeffect.h"

#include "mimedata.h"
#include "dragdropevent.h"
#include "dropitem.h"

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QGraphicsEffect>

#undef signals
extern "C" {
#include <libwnck/util.h>
}

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
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 0, 1, "QSortFilterProxyModelQML");

    qmlRegisterType<WindowInfo>(uri, 0, 1, "WindowInfo");
    qmlRegisterType<WindowsList>(uri, 0, 1, "WindowsList");
    qmlRegisterType<ScreenInfo>(); // Register the type as non creatable
    qmlRegisterType<WorkspacesInfo>(); // Register the type as non creatable

    qmlRegisterType<CacheEffect>(uri, 0, 1, "CacheEffect");
    qmlRegisterType<QGraphicsBlurEffect>("Effects", 1, 0, "Blur");
    qmlRegisterType<QGraphicsColorizeEffect>("Effects", 1, 0, "Colorize");
    qmlRegisterType<QGraphicsDropShadowEffect>("Effects", 1, 0, "DropShadow");

    /* Custom drag’n’drop implementation in QML */
    qmlRegisterType<DeclarativeDropItem>(uri, 0, 1, "DropItem");
    qmlRegisterType<DeclarativeMimeData>();
    qmlRegisterType<DeclarativeDragDropEvent>();
}

void Unity2dPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    XSetErrorHandler(_x_errhandler);

    engine->addImageProvider(QString("blended"), new BlendedImageProvider);
    engine->addImageProvider(QString("window"), new WindowImageProvider);

    /* ScreenInfo is exposed as a context property as it's a singleton and therefore
       not creatable directly in QML */
    engine->rootContext()->setContextProperty("screen", ScreenInfo::instance());

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
}

Q_EXPORT_PLUGIN2(Unity2d, Unity2dPlugin);
