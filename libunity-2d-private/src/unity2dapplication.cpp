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
#include <gconnector.h>
#include <gscopedpointer.h>
#include <unity2ddebug.h>
#include <unity2dtr.h>

// Qt
#include <QFont>
#include <QWindowsStyle>
#include <QAccessible>
#include <QAccessibleWidget>
#include <QWidget>

// GTK
#include <gtk/gtk.h>
#include <pango/pango.h>

// Testability
#include <QtPlugin>
#include <QPluginLoader>
#include <QLibraryInfo>
#include "testabilityinterface.h"

// libc
#include <stdlib.h>

///////////////////////////////
class PlatformFontTracker
{
public:
    PlatformFontTracker()
    {
        m_gConnector.connect(gtk_settings_get_default(), "notify::gtk-font-name",
            G_CALLBACK(PlatformFontTracker::onFontChanged), this);

        updateFont();
    }

private:
    void updateFont()
    {
        gchar* fontName = 0;
        g_object_get(gtk_settings_get_default(), "gtk-font-name", &fontName, NULL);
        GScopedPointer<PangoFontDescription, pango_font_description_free> fontDescription(
            pango_font_description_from_string(fontName)
            );
        g_free(fontName);

        int size = pango_font_description_get_size(fontDescription.data());

        QFont font = QFont(
            pango_font_description_get_family(fontDescription.data()),
            size / PANGO_SCALE
            );

        QApplication::setFont(font);
    }

    static void onFontChanged(GObject*, GParamSpec*, PlatformFontTracker* obj)
    {
        obj->updateFont();
    }

    GConnector m_gConnector;
};

///////////////////////////////
AbstractX11EventFilter::~AbstractX11EventFilter()
{
    Unity2dApplication* application = Unity2dApplication::instance();
    if (application != NULL) {
        application->removeX11EventFilter(this);
    }
}

///////////////////////////////
static bool arrayContains(char** begin, char** end, const char* string)
{
    for (char** ptr = begin; ptr != end; ++ptr) {
        if (strcmp(*ptr, string) == 0) {
            return true;
        }
    }
    return false;
}

QAccessibleInterface *panelFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = 0;

    if (classname == "Unity2dPanel" && object && object->isWidgetType()) {
        interface = new QAccessibleWidget(static_cast<QWidget *>(object), QAccessible::ToolBar);
    }

    return interface;
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

    /* We have these if we were DBus activated, but we don't want our child
     * processes to inherit them
     *
     * https://launchpad.net/bugs/873027
     */
    unsetenv("DBUS_STARTER_ADDRESS");
    unsetenv("DBUS_STARTER_BUS_TYPE");

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
, m_platformFontTracker(new PlatformFontTracker)
{
    /* Load Testability Plugin on startup if requested */
    if (arrayContains(argv, argv + argc, "-testability")) {
        loadTestabilityPlugin();
    }

    /* Configure translations */
    Unity2dTr::init("unity-2d", INSTALL_PREFIX "/share/locale");

    /* TRANSLATORS: This refers to the direction of text
       (left-to-right or right-to-left): possible values are LTR or RTL */
    if (u2dTr("QT_LAYOUT_DIRECTION") == "RTL") {
        QApplication::setLayoutDirection(Qt::RightToLeft);
    }

    /* Allow developers to run Unity 2D uninstalled by telling dconf-qt
       where to look for Unity 2D's schemas.
       It relies on the fact that the schema is compiled when running cmake.
    */
    if (!isRunningInstalled()) {
        qputenv("GSETTINGS_SCHEMA_DIR", unity2dDirectory().toLocal8Bit() + "/data");
    }

    QAccessible::installFactory(panelFactory);
}

Unity2dApplication::~Unity2dApplication()
{
    qDeleteAll(m_x11EventFilters);
    delete m_platformFontTracker;
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

/*
 * Load the Testability Plugin if available
 *
 * Testability is a tool required for UI testing. See tests/ directory.
 */
void Unity2dApplication::loadTestabilityPlugin()
{
     QString testabilityPlugin = "testability/libtestability";
     QString testabilityPluginPostfix = ".so";

     testabilityPlugin = QLibraryInfo::location(QLibraryInfo::PluginsPath)
                         + QObject::tr("/") + testabilityPlugin + testabilityPluginPostfix;
     QPluginLoader loader(testabilityPlugin.toLatin1().data());

     QObject *plugin = loader.instance();
     if (plugin) {
         qDebug("Testability plugin loaded successfully!");
         testabilityInterface = qobject_cast<TestabilityInterface *>(plugin);

         if (testabilityInterface) {
             qDebug("Testability interface obtained!");
             testabilityInterface->Initialize();
         } else {
             qDebug("Failed to get testability interface!");
         }
     } else {
         qDebug("Testability plugin %s load failed with error:%s",
                testabilityPlugin.toLatin1().data(), loader.errorString().toLatin1().data());
     }
}

#include <unity2dapplication.moc>
