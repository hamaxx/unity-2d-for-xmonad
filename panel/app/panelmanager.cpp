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
#include "panelmanager.h"

// Local
#include <config.h>
#include <indicatorsmanager.h>

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

static QPalette getPalette()
{
    QPalette palette;

    /* Should use the panel's background provided by Unity but it turns
       out not to be good. It would look like:

         QBrush bg(QPixmap("theme:/panel_background.png"));
    */
    QBrush bg(QPixmap(unity2dDirectory() + "/panel/artwork/background.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    return palette;
}

static QLabel* createSeparator()
{
    QLabel* label = new QLabel;
    QPixmap pix(unity2dDirectory() + "/panel/artwork/divider.png");
    label->setPixmap(pix);
    label->setFixedSize(pix.size());
    return label;
}

PanelManager::PanelManager(QObject* parent)
: QObject(parent)
, m_indicatorsManager(new IndicatorsManager(this))
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

PanelManager::~PanelManager()
{
    qDeleteAll(m_panels);
}

Unity2dPanel* PanelManager::instantiatePanel(int screen)
{
    Unity2dPanel* panel = new Unity2dPanel;
    panel->setEdge(Unity2dPanel::TopEdge);
    panel->setPalette(getPalette());
    panel->setFixedHeight(24);

    int leftmost = QApplication::desktop()->screenNumber(QPoint());
    if (screen == leftmost) {
        panel->addWidget(new HomeButtonApplet);
        panel->addWidget(createSeparator());
    }
    if (screen == leftmost) {
        /* It doesn’t make sense to have more than one instance of the systray,
           XEmbed’ed windows can be displayed only once anyway. */
        panel->addWidget(new LegacyTrayApplet);
    }
    panel->addWidget(new IndicatorApplet(m_indicatorsManager));
    return panel;
}

void
PanelManager::onScreenCountChanged(int newCount)
{
    if (newCount > 0) {
        QDesktopWidget* desktop = QApplication::desktop();
        int size = m_panels.size();
        Unity2dPanel* panel;

        /* The first panel is always the one on the leftmost screen. */
        int leftmost = desktop->screenNumber(QPoint());
        if (size > 0) {
            panel = m_panels[0];
        } else {
            panel = instantiatePanel(leftmost);
            m_panels.append(panel);
        }
        panel->show();
        panel->move(desktop->screenGeometry(leftmost).topLeft());

        /* Update the position of other existing panels, and instantiate new
           panels as needed. */
        int i = 1;
        for (int screen = 0; screen < newCount; ++screen) {
            if (screen == leftmost) {
                continue;
            }
            if (i < size) {
                panel = m_panels[i];
            } else {
                panel = instantiatePanel(screen);
                m_panels.append(panel);
            }
            panel->show();
            panel->move(desktop->screenGeometry(screen).topLeft());
            ++i;
        }
    }
    /* Remove extra panels if any. */
    while (m_panels.size() > newCount) {
        delete m_panels.takeLast();
    }
}

#include "panelmanager.moc"

