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
#include <QColor>
#include <QFont>
#include <QPalette>

// GTK
#include <gtk/gtk.h>
#include <pango/pango.h>

static const char* METACITY_THEME_DIR = "/usr/share/themes/%1/metacity-1";

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
    GObjectScopedPointer<GtkStyleContext> m_styleContext;
    GConnector m_gConnector;

    QColor m_textColor;
    QColor m_backgroundTopColor;
    QColor m_backgroundBottomColor;
    QColor m_textShadowColor;
    QColor m_lineColor;
    QFont m_font;
    QString m_themeName;

    static void onThemeChanged(GObject*, GParamSpec*, gpointer data)
    {
        PanelStylePrivate* priv = reinterpret_cast<PanelStylePrivate*>(data);
        priv->updatePalette();
    }

    static void onFontChanged(GObject*, GParamSpec*, gpointer data)
    {
        PanelStylePrivate* priv = reinterpret_cast<PanelStylePrivate*>(data);
        priv->updateFont();
    }

    void updatePalette()
    {
        gchar* themeName = 0;
        g_object_get(gtk_settings_get_default(), "gtk-theme-name", &themeName, NULL);
        m_themeName = QString::fromUtf8(themeName);
        g_free(themeName);

        GtkStyleContext* context = m_styleContext.data();
        gtk_style_context_invalidate(context);

        m_textColor             = colorFromContext(gtk_style_context_get_color, context, GTK_STATE_FLAG_NORMAL);
        m_textShadowColor       = colorFromContext(gtk_style_context_get_color, context, GTK_STATE_FLAG_SELECTED);
        m_lineColor             = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_NORMAL).darker(130);
        m_backgroundTopColor    = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_ACTIVE);
        m_backgroundBottomColor = colorFromContext(gtk_style_context_get_background_color, context, GTK_STATE_FLAG_NORMAL);

        QPalette pal;
        pal.setBrush(QPalette::Window, generateBackgroundBrush());
        pal.setColor(QPalette::Text, m_textColor);
        pal.setColor(QPalette::WindowText, m_textColor);
        pal.setColor(QPalette::ButtonText, m_textColor);
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

    void updateFont()
    {
        gchar* fontName = 0;
        g_object_get(gtk_settings_get_default(), "gtk-font-name", &fontName, NULL);
        GScopedPointer<PangoFontDescription, pango_font_description_free> fontDescription(
            pango_font_description_from_string(fontName)
            );
        g_free(fontName);

        int size = pango_font_description_get_size(fontDescription.data());

        m_font = QFont(
            pango_font_description_get_family(fontDescription.data()),
            size / PANGO_SCALE
            );

        QApplication::setFont(m_font);
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
    d->m_gConnector.connect(gtk_settings_get_default(), "notify::gtk-font-name",
        G_CALLBACK(PanelStylePrivate::onFontChanged), d);

    d->updatePalette();
    d->updateFont();
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

GtkStyleContext* PanelStyle::styleContext() const
{
    return d->m_styleContext.data();
}

QPixmap PanelStyle::windowButtonPixmap(PanelStyle::WindowButtonType type, PanelStyle::WindowButtonState state)
{
    QString dir = QString(METACITY_THEME_DIR).arg(d->m_themeName);

    QString typeString, stateString;
    switch (type) {
    case CloseWindowButton:
        typeString = "close";
        break;
    case MinimizeWindowButton:
        typeString = "minimize";
        break;
    case UnmaximizeWindowButton:
        typeString = "unmaximize";
        break;
    }

    switch (state) {
    case NormalState:
        stateString = "";
        break;
    case PrelightState:
        stateString = "_focused_prelight";
        break;
    case PressedState:
        stateString = "_focused_pressed";
        break;
    }

    QString path = QString("%1/%2%3.png")
        .arg(dir)
        .arg(typeString)
        .arg(stateString);
    return QPixmap(path);
}

#include "panelstyle.moc"
