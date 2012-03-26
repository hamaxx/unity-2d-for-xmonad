/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "panelpalettemanager.h"

// libunity-2d
#include <cairoutils.h>
#include <dashclient.h>
#include <hudclient.h>
#include <panelstyle.h>
#include <unity2dpanel.h>
#include <config.h>

// Qt
#include <QBrush>
#include <QPalette>
#include <QVariant>

// GTK
#include <gtk/gtk.h>

namespace Unity2d
{

static void onThemeChanged(GObject*, GParamSpec*, gpointer data)
{
    PanelPaletteManager* priv = reinterpret_cast<PanelPaletteManager*>(data);
    priv->updatePalette();
}

PanelPaletteManager::PanelPaletteManager(Unity2dPanel* panel)
 : QObject(panel), m_panel(panel)
{
    connect(DashClient::instance(), SIGNAL(activeChanged(bool)), this, SLOT(updatePalette()));
    connect(DashClient::instance(), SIGNAL(dashDisconnected()), this, SLOT(updatePalette()));
    connect(HUDClient::instance(), SIGNAL(activeChanged(bool)), this, SLOT(updatePalette()));
    connect(DashClient::instance(), SIGNAL(screenChanged(int)), this, SLOT(updatePalette()));
    connect(HUDClient::instance(), SIGNAL(screenChanged(int)), this, SLOT(updatePalette()));
    connect(&unity2dConfiguration(), SIGNAL(averageBgColorChanged(QString)), this, SLOT(updatePalette()));

    m_gConnector.connect(gtk_settings_get_default(), "notify::gtk-theme-name", G_CALLBACK(onThemeChanged), this);
    updatePalette();
}

static QBrush generateBackgroundBrush()
{
    QImage image(100, 24, QImage::Format_ARGB32_Premultiplied); // FIXME: Hardcoded
    image.fill(Qt::transparent);
    CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(&image));
    CairoUtils::Pointer cr(cairo_create(surface.data()));
    GtkStyleContext* context = PanelStyle::instance()->styleContext();
    gtk_render_background(context, cr.data(), 0, 0, image.width(), image.height());
    gtk_render_frame(context, cr.data(), 0, 0, image.width(), image.height());
    return QBrush(image);
}

void PanelPaletteManager::updatePalette()
{
    GtkStyleContext* context = PanelStyle::instance()->styleContext();
    gtk_style_context_invalidate(context);

    // Without this line, it seems the GtkStyleContext is not correctly
    // initialized and we get some uninitialized pixels in the background
    // brush.
    gtk_style_context_get(context, GTK_STATE_FLAG_NORMAL, NULL);

    QPalette pal;
    if (DashClient::instance()->activeInScreen(m_panel->screen()) || HUDClient::instance()->activeInScreen(m_panel->screen())) {
        /* The background color is the same as in the launcher */
        QColor wallpaperColor(unity2dConfiguration().property("averageBgColor").toString());
        QColor backgroundColor(wallpaperColor.red(), wallpaperColor.green(), wallpaperColor.blue(), 168);
        backgroundColor = backgroundColor.darker(800);
        pal.setBrush(QPalette::Window, backgroundColor);
    } else {
        pal.setBrush(QPalette::Window, generateBackgroundBrush());
    }
    m_panel->setPalette(pal);
}

} // namespace Unity2d

#include "panelpalettemanager.moc"
