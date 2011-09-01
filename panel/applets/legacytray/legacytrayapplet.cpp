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

// Self
#include "legacytrayapplet.h"

// Local
#include "fdoselectionmanager.h"
#include "fdotask.h"

// libunity-2d-private
#include <debug_p.h>

// libdconf-qt
#include <qconf.h>

// Qt
#include <QApplication>
#include <QHBoxLayout>
#include <QVariant>

#define PANEL_DCONF_SCHEMA QString("com.canonical.Unity.Panel")

LegacyTrayApplet::LegacyTrayApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
, m_selectionManager(new SystemTray::FdoSelectionManager)
, m_dconfPanel(new QConf(PANEL_DCONF_SCHEMA))
{
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    connect(m_selectionManager, SIGNAL(taskCreated(SystemTray::Task*)),
        SLOT(slotTaskCreated(SystemTray::Task*)));

    m_whitelist = m_dconfPanel->property("systrayWhitelist").toStringList();
}

LegacyTrayApplet::~LegacyTrayApplet()
{
    delete m_selectionManager;
    delete m_dconfPanel;
}

void LegacyTrayApplet::slotTaskCreated(SystemTray::Task* task)
{
    /* Only accept tasks whose name is in the whitelist.
       The whitelist contains a "List of client names, resource classes or wm
       classes to allow in the Panel's systray implementation." but here we only
       support matching on WM_CLASS.
    */
    if (!m_whitelist.contains(task->name())) {
        return;
    }

    task->createWidget();
    connect(task, SIGNAL(widgetCreated(QWidget*)), SLOT(slotWidgetCreated(QWidget*)));
}

void LegacyTrayApplet::slotWidgetCreated(QWidget* widget)
{
    layout()->addWidget(widget);
}

#include "legacytrayapplet.moc"
