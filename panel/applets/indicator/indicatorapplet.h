/*
 * This file is part of unity-2d
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INDICATORAPPLET_H
#define INDICATORAPPLET_H

// Local
#include <applet.h>

// Qt
#include <QDBusInterface>
#include <QMenuBar>

class QX11EmbedContainer;

struct _IndicatorPlugin;

class IndicatorApplet : public Unity2d::Applet
{
Q_OBJECT
public:
    IndicatorApplet();

private Q_SLOTS:
    void loadIndicators();
    void slotActionAdded(QAction*);
    void slotActionRemoved(QAction*);
    void createGtkIndicator();
    void adjustGtkIndicatorSize();

private:
    Q_DISABLE_COPY(IndicatorApplet)

    QDBusInterface* m_watcher;
    QMenuBar* m_menuBar;
    QX11EmbedContainer* m_container;
    struct _IndicatorPlugin* m_gtkIndicator;

    void setupUi();
};

#endif /* INDICATORAPPLET_H */
