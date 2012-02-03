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

/*
 * Modified by:
 * - Jure Ham <jure@hamsworld.net>
 */

// Self
#include "legacytrayapplet.h"

// Local
#include "fdoselectionmanager.h"
#include "fdotask.h"

// libunity-2d-private
#include <debug_p.h>
#include <config.h>

// Qt
#include <QApplication>
#include <QHBoxLayout>
#include <QVariant>

LegacyTrayApplet::LegacyTrayApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
, m_selectionManager(new SystemTray::FdoSelectionManager)
{
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    connect(m_selectionManager, SIGNAL(taskCreated(SystemTray::Task*)),
        SLOT(slotTaskCreated(SystemTray::Task*)));

    m_whitelist = panelConfiguration().property("systrayWhitelist").toStringList();
    m_whitelistAll = m_whitelist.contains("all", Qt::CaseInsensitive);
}

LegacyTrayApplet::~LegacyTrayApplet()
{
    delete m_selectionManager;
}

void LegacyTrayApplet::slotTaskCreated(SystemTray::Task* task)
{
    /* Only accept tasks whose name is in the whitelist.
       The whitelist contains a "List of client names, resource classes or wm
       classes to allow in the Panel's systray implementation." but here we only
       support matching on WM_CLASS.
    */
    if (!m_whitelistAll && !m_whitelist.contains(task->name(), Qt::CaseInsensitive)) {
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
