/*
 * This file is part of unity-2d
 *
 * Copyright 2010, 2012 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Albert Astals Cid <albert.astals@canonical.com>
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

#ifndef STRUTMANAGER_H
#define STRUTMANAGER_H

// Unity 2D
#include "unity2dpanel.h"

class StrutManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QObject* widget READ widget WRITE setWidget NOTIFY widgetChanged)
    Q_PROPERTY(Unity2dPanel::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

public:
    StrutManager();
    ~StrutManager();

    bool enabled() const;
    void setEnabled(bool value);

    // QObject due to QML constraints
    QObject *widget() const;
    void setWidget(QObject *widget);

    Unity2dPanel::Edge edge() const;
    void setEdge(Unity2dPanel::Edge edge);

    /**
     * Returns the width set to the strut manager
     * If is -1 (the default value) will use the width of the widget given in setWidget
     */
    int width() const;
    void setWidth(int width);
    /**
     * Returns the actual width in use by the strut manager
     * It is either width() or m_widget->width()
     */
    int realWidth() const;

    /**
     * Returns the height set to the strut manager
     * If is -1 (the default value) will use the height of the widget given in setWidget
     */
    int height() const;
    void setHeight(int height);
    /**
     * Returns the actual height in use by the strut manager
     * It is either width() or m_widget->width()
     */
    int realHeight() const;

Q_SIGNALS:
    void enabledChanged(bool enabled);
    void widgetChanged(QObject *widget);
    void edgeChanged(Unity2dPanel::Edge edge);
    void widthChanged(int width);
    void heightChanged(int height);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private Q_SLOTS:
    void updateStrut();

private:
    void reserveStrut();
    void releaseStrut();

    bool m_enabled;
    QWidget *m_widget;
    Unity2dPanel::Edge m_edge;
    int m_width;
    int m_height;
};

#endif /* STRUTMANAGER_H */