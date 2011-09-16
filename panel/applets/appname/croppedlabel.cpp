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
#include "croppedlabel.h"

// Local
#include <cairoutils.h>
#include <panelstyle.h>

// unity-2d
#include <debug_p.h>

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// Qt
#include <QApplication>
#include <QImage>
#include <QPainter>

// GTK
#include <gtk/gtk.h>

static const int FADEOUT_WIDTH = 30;

static const char* WINDOW_TITLE_FONT_KEY = "/apps/metacity/general/titlebar_font";

CroppedLabel::CroppedLabel(QWidget* parent)
: QLabel(parent)
, m_gconfItem(new GConfItemQmlWrapper(this))
{
    QObject::connect(m_gconfItem, SIGNAL(valueChanged()),
                     this, SLOT(onWindowTitleFontNameChanged()));
    m_gconfItem->setKey(WINDOW_TITLE_FONT_KEY);
    m_windowTitleFontName = m_gconfItem->getValue().toString();
}

QSize CroppedLabel::minimumSizeHint() const
{
    return QWidget::minimumSizeHint();
}

static void paintFadeoutGradient(QImage* image)
{
    QPainter painter(image);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    bool isLeftToRight = QApplication::isLeftToRight();
    QRect gradientRect(isLeftToRight ? image->width() - FADEOUT_WIDTH : 0, 0,
                       FADEOUT_WIDTH, image->height());
    QLinearGradient gradient(gradientRect.topLeft(), gradientRect.topRight());
    gradient.setColorAt(0, isLeftToRight ? Qt::white : Qt::transparent);
    gradient.setColorAt(1, isLeftToRight ? Qt::transparent : Qt::white);
    painter.fillRect(gradientRect, gradient);
}

void CroppedLabel::paintEvent(QPaintEvent* event)
{
    // Create an image filled with background brush (to avoid subpixel hinting
    // artefacts around text)
    QImage image(width(), height(), QImage::Format_ARGB32_Premultiplied);
    {
        QPainter painter(&image);
        painter.initFrom(this);
        painter.eraseRect(rect());
    }

    // Create a pango layout
    GObjectScopedPointer<PangoContext> pangoContext(gdk_pango_context_get());
    GObjectScopedPointer<PangoLayout> layout(pango_layout_new(pangoContext.data()));

    // Set font
    QByteArray fontName = m_windowTitleFontName.toUtf8();
    PangoFontDescription* desc = pango_font_description_from_string(fontName.data());
    pango_layout_set_font_description(layout.data(), desc);
    pango_font_description_free(desc);

    // Set text
    QByteArray utf8Text = text().toUtf8();
    pango_layout_set_text (layout.data(), utf8Text.data(), -1);

    // Get text size
    int textWidth = 0;
    int textHeight = 0;
    pango_layout_get_pixel_size(layout.data(), &textWidth, &textHeight);

    // Draw text
    CairoUtils::SurfacePointer surface(CairoUtils::createSurfaceForQImage(&image));
    CairoUtils::Pointer cr(cairo_create(surface.data()));

    PanelStyle* style = PanelStyle::instance();
    GtkStyleContext* style_context = style->styleContext();

    gtk_style_context_save(style_context);

    GtkWidgetPath* widget_path = gtk_widget_path_new();
    gtk_widget_path_append_type(widget_path, GTK_TYPE_MENU_BAR);
    gtk_widget_path_append_type(widget_path, GTK_TYPE_MENU_ITEM);
    gtk_widget_path_iter_set_name(widget_path, -1 , "UnityPanelWidget");

    gtk_style_context_set_path(style_context, widget_path);
    gtk_style_context_add_class(style_context, GTK_STYLE_CLASS_MENUBAR);
    gtk_style_context_add_class(style_context, GTK_STYLE_CLASS_MENUITEM);

    gtk_render_layout(style_context, cr.data(),
        QApplication::isLeftToRight() ?
            contentsRect().left() :
            contentsRect().right() - textWidth,
        contentsRect().top() + (height() - textHeight) / 2,
        layout.data());

    gtk_widget_path_free(widget_path);

    gtk_style_context_restore(style_context);

    // Fade if necessary
    if (textWidth > contentsRect().width()) {
        paintFadeoutGradient(&image);
    }

    // Paint on our widget
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

void CroppedLabel::onWindowTitleFontNameChanged()
{
    m_windowTitleFontName = m_gconfItem->getValue().toString();
    update();
}

#include "croppedlabel.moc"
