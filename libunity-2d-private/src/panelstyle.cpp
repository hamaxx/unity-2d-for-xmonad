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
#include <QStyle>

// GTK
#include <gtk/gtk.h>

static const char* METACITY_THEME_DIR = "/usr/share/themes/%1/metacity-1";

class PanelStylePrivate
{
public:
    PanelStyle* q;
    GObjectScopedPointer<GtkStyleContext> m_styleContext;
    GConnector m_gConnector;

    QString m_themeName;

    static void onThemeChanged(GObject*, GParamSpec*, gpointer data)
    {
        PanelStylePrivate* priv = reinterpret_cast<PanelStylePrivate*>(data);
        priv->updatePalette();
    }

    void updatePalette()
    {
        gchar* themeName = 0;
        g_object_get(gtk_settings_get_default(), "gtk-theme-name", &themeName, NULL);
        m_themeName = QString::fromUtf8(themeName);
        g_free(themeName);

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
        gtk_render_frame(m_styleContext.data(), cr.data(), 0, 0, image.width(), image.height());
        return QBrush(image);
    }

    QPixmap windowButtonPixmapFromWMTheme(PanelStyle::WindowButtonType type, PanelStyle::WindowButtonState state)
    {
        QString dir = QString(METACITY_THEME_DIR).arg(m_themeName);

        QString typeString, stateString;
        switch (type) {
        case PanelStyle::CloseWindowButton:
            typeString = "close";
            break;
        case PanelStyle::MinimizeWindowButton:
            typeString = "minimize";
            break;
        case PanelStyle::UnmaximizeWindowButton:
            typeString = "unmaximize";
            break;
        }

        switch (state) {
        case PanelStyle::NormalState:
            stateString = "";
            break;
        case PanelStyle::PrelightState:
            stateString = "_focused_prelight";
            break;
        case PanelStyle::PressedState:
            stateString = "_focused_pressed";
            break;
        }

        QString path = QString("%1/%2%3.png")
            .arg(dir)
            .arg(typeString)
            .arg(stateString);
        return QPixmap(path);
    }

    QPixmap genericWindowButtonPixmap(PanelStyle::WindowButtonType type, PanelStyle::WindowButtonState state)
    {
        QStyle::StandardPixmap standardIcon;
        switch (type) {
        case PanelStyle::CloseWindowButton:
            standardIcon = QStyle::SP_TitleBarCloseButton;
            break;
        case PanelStyle::MinimizeWindowButton:
            standardIcon = QStyle::SP_TitleBarMinButton;
            break;
        case PanelStyle::UnmaximizeWindowButton:
            standardIcon = QStyle::SP_TitleBarNormalButton;
            break;
        }

        QIcon icon = QApplication::style()->standardIcon(standardIcon);
        const int extent = 22;
        switch (state) {
        case PanelStyle::NormalState:
            return icon.pixmap(extent);
        case PanelStyle::PrelightState:
            return icon.pixmap(extent, QIcon::Active);
        case PanelStyle::PressedState:
            return icon.pixmap(extent, QIcon::Active, QIcon::On);
        }
        // Silence compiler
        return QPixmap();
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

QPixmap PanelStyle::windowButtonPixmap(PanelStyle::WindowButtonType type, PanelStyle::WindowButtonState state)
{
    // According to Unity PanelStyle code, the buttons of some WM themes do not
    // match well with the panel background. So except for themes we provide,
    // fallback to generic button pixmaps.
    if (d->m_themeName == "Ambiance" || d->m_themeName == "Radiance") {
        return d->windowButtonPixmapFromWMTheme(type, state);
    } else {
        return d->genericWindowButtonPixmap(type, state);
    }
}

#include "panelstyle.moc"
