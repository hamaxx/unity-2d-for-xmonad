/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Olivier Tilloy <olivier.tilloy@canonical.com>
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
#include "panelsmanager.h"

// Local
#include <config.h>

// Applets
#include <appindicator/appindicatorapplet.h>
#include <appname/appnameapplet.h>
#include <homebutton/homebuttonapplet.h>
#include <indicator/indicatorapplet.h>
#include <legacytray/legacytrayapplet.h>

// Unity
#include <unity2dpanel.h>

// Qt
#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

using namespace Unity2d;

QPalette getPalette()
{
    QPalette palette;

    /* Should use the panel's background provided by Unity but it turns
       out not to be good. It would look like:

         QBrush bg(QPixmap("theme:/panel_background.png"));
    */
    QBrush bg(QPixmap(unity2dDirectory() + "/panel/artwork/background.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);
    return palette;
}

QLabel* createSeparator()
{
    QLabel* label = new QLabel;
    QPixmap pix(unity2dDirectory() + "/panel/artwork/divider.png");
    label->setPixmap(pix);
    label->setFixedSize(pix.size());
    return label;
}

struct PanelsManagerPrivate
{
    QList<Unity2dPanel*> m_panels;

    Unity2dPanel* instantiatePanel(int screen) const
    {
        Unity2dPanel* panel = new Unity2dPanel;
        panel->setEdge(Unity2dPanel::TopEdge);
        panel->setPalette(getPalette());
        panel->setFixedHeight(24);

        if (screen == QApplication::desktop()->primaryScreen()) {
            panel->addWidget(new HomeButtonApplet);
            panel->addWidget(createSeparator());
        }
        panel->addWidget(new AppNameApplet);
        panel->addWidget(new LegacyTrayApplet);
        panel->addWidget(new IndicatorApplet);
        return panel;
    }
};

PanelsManager::PanelsManager(QObject* parent)
    : QObject(parent)
    , d(new PanelsManagerPrivate)
{
    QDesktopWidget* desktop = QApplication::desktop();
    for(int i = 0; i < desktop->screenCount(); ++i) {
        Unity2dPanel* panel = d->instantiatePanel(i);
        d->m_panels.append(panel);
        panel->move(desktop->screenGeometry(i).topLeft());
        panel->show();
    }
}

PanelsManager::~PanelsManager()
{
    qDeleteAll(d->m_panels);
    delete d;
}

