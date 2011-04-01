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

Unity2dPanel* instantiatePanel(int screen)
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

PanelsManager::PanelsManager(QObject* parent)
    : QObject(parent)
{
    QDesktopWidget* desktop = QApplication::desktop();
    for(int i = 0; i < desktop->screenCount(); ++i) {
        Unity2dPanel* panel = instantiatePanel(i);
        m_panels.append(panel);
        panel->show();
        panel->move(desktop->screenGeometry(i).topLeft());
    }
    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));
}

PanelsManager::~PanelsManager()
{
    qDeleteAll(m_panels);
}

void
PanelsManager::onScreenCountChanged(int newCount)
{
    QDesktopWidget* desktop = QApplication::desktop();
    int size = m_panels.size();
    /* Update the position of existing panels, and instantiate new panels. */
    for (int i = 0; i < newCount; ++i) {
        Unity2dPanel* panel;
        if (i < size) {
            panel = m_panels[i];
        } else {
            panel = instantiatePanel(i);
            m_panels.append(panel);
        }
        panel->show();
        panel->move(desktop->screenGeometry(i).topLeft());
    }
    /* Remove extra panels if any. */
    while (m_panels.size() > newCount) {
        delete m_panels.takeLast();
    }
}

#include "panelsmanager.moc"

