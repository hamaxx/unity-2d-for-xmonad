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
#ifndef INDICATORENTRYWIDGET_H
#define INDICATORENTRYWIDGET_H

// Local

// libunity-core
#include <UnityCore/IndicatorEntry.h>

// Qt
#include <QWidget>

struct _GtkWidgetPath;
struct _PangoLayout;

class QPainter;

class IndicatorEntryWidget : public QWidget, public sigc::trackable
{
Q_OBJECT
public:
    IndicatorEntryWidget(const unity::indicator::Entry::Ptr& entry);
    ~IndicatorEntryWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setPadding(int);

    bool isEmpty() const;

    unity::indicator::Entry::Ptr entry() const;

    /**
     * Shows the menu.
     *
     * When this method is called because of a click, button
     * must be set to Qt::LeftButton.
     *
     * When it is called because user previously clicked an indicator and moved the
     * mouse to another indicator, button must be set to Qt::NoButton.
     */
    void showMenu(Qt::MouseButton button);

Q_SIGNALS:
    void isEmptyChanged();

protected:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);
    bool event(QEvent*);

private:
    unity::indicator::Entry::Ptr m_entry;
    QPixmap m_pix;
    int m_padding;
    bool m_hasIcon;
    bool m_hasLabel;
    struct _GtkWidgetPath* m_gtkWidgetPath;
    void updatePix();
    QPixmap decodeIcon();
    void paintActiveBackground(QImage*);

    struct _PangoLayout* createPangoLayout();
    void paintLabel(QImage*, struct _PangoLayout*, int labelX);
};

#endif /* INDICATORENTRYWIDGET_H */
