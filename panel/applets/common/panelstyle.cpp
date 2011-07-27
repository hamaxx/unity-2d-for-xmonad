/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Self
#include "panelstyle.h"

// libunity-2d
#include <cairoutils.h>
#include <debug_p.h>
#include <gconnector.h>
#include <gscopedpointer.h>

// Qt
#include <QApplication>
#include <QPalette>

// GTK
#include <gtk/gtk.h>

class PanelStylePrivate
{
public:
    PanelStyle* q;
    GObjectScopedPointer<GtkStyleContext> m_styleContext;
    GConnector m_gConnector;

    static void onThemeChanged(GObject*, GParamSpec*, gpointer data)
    {
        PanelStylePrivate* priv = reinterpret_cast<PanelStylePrivate*>(data);
        priv->updatePalette();
    }

    void updatePalette()
    {
        GtkStyleContext* context = m_styleContext.data();
        gtk_style_context_invalidate(context);

        // Without this line, it seems the GtkStyleContext is not correctly
        // initialized and we get some uninitialized pixels in the background
        // brush.
        gtk_style_context_get(context, GTK_STATE_FLAG_NORMAL, NULL);

        QPalette pal;
        pal.setBrush(QPalette::Window, generateBackgroundBrush());
        QApplication::setPalette(pal);
    }

    QBrush generateBackgroundBrush()
    {
        QImage image(100, 24, QImage::Format_ARGB32_Premultiplied); // FIXME: Hardcoded
        image.fill(Qt::transparent);
        CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(&image));
        CairoUtils::Pointer cr(cairo_create(surface.data()));
        gtk_render_background(m_styleContext.data(), cr.data(), 0, 0, image.width(), image.height());
        return QBrush(image);
    }
};

PanelStyle::PanelStyle(QObject* parent)
: d(new PanelStylePrivate)
{
    d->q = this;
    d->m_styleContext.reset(gtk_style_context_new());

    GtkWidgetPath* widgetPath = gtk_widget_path_new ();
    gtk_widget_path_append_type(widgetPath, GTK_TYPE_WINDOW);
    gtk_widget_path_iter_set_name(widgetPath, -1 , "UnityPanelWidget");

    gtk_style_context_set_path(d->m_styleContext.data(), widgetPath);
    gtk_style_context_add_class(d->m_styleContext.data(), "gnome-panel-menu-bar");
    gtk_style_context_add_class(d->m_styleContext.data(), "unity-panel");

    gtk_widget_path_free (widgetPath);

    d->m_gConnector.connect(gtk_settings_get_default(), "notify::gtk-theme-name",
        G_CALLBACK(PanelStylePrivate::onThemeChanged), d);

    d->updatePalette();
}

PanelStyle::~PanelStyle()
{
    delete d;
}

PanelStyle* PanelStyle::instance()
{
    static PanelStyle style;
    return &style;
}

GtkStyleContext* PanelStyle::styleContext() const
{
    return d->m_styleContext.data();
}

#include "panelstyle.moc"
