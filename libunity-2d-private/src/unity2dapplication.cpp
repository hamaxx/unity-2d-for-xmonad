/*
 * Unity2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

// Self
#include "unity2dapplication.h"
#include "config.h"

// libunity-2d
#include <debug_p.h>
#include <unity2ddebug.h>

// Qt
#include <QWindowsStyle>

// GTK
#include <gtk/gtk.h>

AbstractX11EventFilter::~AbstractX11EventFilter()
{
    Unity2dApplication* application = Unity2dApplication::instance();
    if (application != NULL) {
        application->removeX11EventFilter(this);
    }
}

static bool arrayContains(char** begin, char** end, const char* string)
{
    for (char** ptr = begin; ptr != end; ++ptr) {
        if (strcmp(*ptr, string) == 0) {
            return true;
        }
    }
    return false;
}

void Unity2dApplication::earlySetup(int& argc, char** argv)
{
    // Parts of unity-2d uses GTK so it needs to be initialized
    gtk_init(&argc, &argv);

    Unity2dDebug::installHandlers();

    /* When the environment variable QT_GRAPHICSSYSTEM is not set, force
     * graphics system to 'raster' instead of the default 'native' which on X11
     * is 'XRender'.  'XRender' defaults to using a TrueColor visual. We do
     * _not_ mimick that behaviour with 'raster' by calling
     * QApplication::setColorSpec because of bugs where some pixmaps become
     * blueish or black rectangular artifacts were appearing randomly:
     *
     *  https://bugs.launchpad.net/unity-2d/+bug/689877
     *  https://bugs.launchpad.net/unity-2d/+bug/734143
     */
    if(getenv("QT_GRAPHICSSYSTEM") == 0) {
        QApplication::setGraphicsSystem("raster");
    }

    /* Unless style has been specified in args, set default Qt style to
     * QWindowStyle to avoid loading QGtkStyle. We don't want to load QGtkStyle
     * because it uses libgtk2, which causes conflicts with our gtk3 code.
     */
    if (!arrayContains(argv, argv + argc, "-style")) {
        QApplication::setStyle(new QWindowsStyle);
    }
}

Unity2dApplication::Unity2dApplication(int& argc, char** argv)
: QApplication(argc, argv)
{
    /* Allow developers to run Unity 2D uninstalled by telling dconf-qt
       where to look for Unity 2D's schemas.
       It relies on the fact that the schema is compiled when running cmake.
    */
    if (!isRunningInstalled()) {
        qputenv("GSETTINGS_SCHEMA_DIR", unity2dDirectory().toLocal8Bit() + "/data");
    }
}

Unity2dApplication::~Unity2dApplication()
{
    qDeleteAll(m_x11EventFilters);
}

Unity2dApplication* Unity2dApplication::instance()
{
    return qobject_cast<Unity2dApplication*>(QCoreApplication::instance());
}

void Unity2dApplication::installX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.append(filter);
}

void Unity2dApplication::removeX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.removeAll(filter);
}

bool Unity2dApplication::x11EventFilter(XEvent* event)
{
    Q_FOREACH(AbstractX11EventFilter* filter, m_x11EventFilters) {
        if (filter->x11EventFilter(event)) {
            return true;
        }
    }
    return QApplication::x11EventFilter(event);
}

#include <unity2dapplication.moc>
