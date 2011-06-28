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

// Local
#include <debug_p.h>
#include <gscopedpointer.h>

// Qt
#include <QColor>
#include <QFont>

// GTK
#include <gtk/gtk.h>
#include <pango/pango.h>

typedef void (*ColorGetter)(GtkStyleContext*, GtkStateFlags, GdkRGBA*);

inline QColor colorFromContext(ColorGetter getter, GtkStyleContext* context, GtkStateFlags state)
{
    GdkRGBA color;
    getter(context, state, &color);
    return QColor::fromRgbF(color.red, color.green, color.blue, color.alpha);
}

class PanelStylePrivate
{
public:
    PanelStyle* q;
    GtkWidget* m_offScreenWindow;
    QString m_themeName;
    gulong m_connection;

    QColor m_textColor;
    QColor m_backgroundTopColor;
    QColor m_backgroundBottomColor;
    QColor m_textShadowColor;
    QColor m_lineColor;
    QFont m_font;

    static void onStyleChanged(GObject*, GParamSpec*, gpointer data)
    {
        PanelStylePrivate* priv = reinterpret_cast<PanelStylePrivate*>(data);
        priv->update();
    }

    void update()
    {
        gchar* themeName = 0;
        g_object_get(gtk_settings_get_default(), "gtk-theme-name", &themeName, NULL);
        m_themeName = QString::fromUtf8(themeName);
        g_free(themeName);

        GtkStyleContext* context = gtk_widget_get_style_context(m_offScreenWindow);

        m_textColor             = colorFromContext(gtk_style_context_get_color, context, GTK_STATE_FLAG_NORMAL);
        m_textShadowColor       = colorFromContext(gtk_style_context_get_color, context, GTK_STATE_FLAG_SELECTED);
        m_lineColor             = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_NORMAL).darker(130);
        m_backgroundTopColor    = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_ACTIVE);
        m_backgroundBottomColor = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_NORMAL);

        updateFont();
    }

    void updateFont()
    {
        GtkSettings* settings = gtk_settings_get_default();
        char* fontName;
        g_object_get(settings, "gtk-font-name", &fontName, NULL);
        GScopedPointer<PangoFontDescription, pango_font_description_free> fontDescription(
            pango_font_description_from_string(fontName)
            );
        g_free(fontName);

        int size = pango_font_description_get_size(fontDescription.data());

        m_font = QFont(
            pango_font_description_get_family(fontDescription.data()),
            size / PANGO_SCALE
            );

        q->changed();
    }
};

PanelStyle::PanelStyle(QObject* parent)
: d(new PanelStylePrivate)
{
    d->q = this;
    d->m_offScreenWindow = gtk_offscreen_window_new();
    gtk_widget_set_name(d->m_offScreenWindow, "UnityPanelWidget");
    gtk_widget_set_size_request(d->m_offScreenWindow, 100, 24);
    gtk_style_context_add_class(gtk_widget_get_style_context(d->m_offScreenWindow), "menubar");
    gtk_widget_show_all(d->m_offScreenWindow);

    d->m_connection = g_signal_connect(gtk_settings_get_default(), "notify::gtk-theme-name",
            G_CALLBACK(PanelStylePrivate::onStyleChanged), this);

    d->update();
}

PanelStyle::~PanelStyle()
{
    gtk_widget_destroy(d->m_offScreenWindow);
    if (d->m_connection) {
        g_signal_handler_disconnect(gtk_settings_get_default(), d->m_connection);
    }
    delete d;
}

PanelStyle* PanelStyle::instance()
{
    static PanelStyle style;
    return &style;
}

QColor PanelStyle::textColor() const
{
    return d->m_textColor;
}

QColor PanelStyle::backgroundTopColor() const
{
    return d->m_backgroundTopColor;
}

QColor PanelStyle::backgroundBottomColor() const
{
    return d->m_backgroundBottomColor;
}

QColor PanelStyle::textShadowColor() const
{
    return d->m_textShadowColor;
}

QColor PanelStyle::lineColor() const
{
    return d->m_lineColor;
}

QFont PanelStyle::font() const
{
    return d->m_font;
}

#include "panelstyle.moc"
