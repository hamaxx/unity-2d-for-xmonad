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

// uqpanel
#include <debug_p.h>

// Qt
#include <QApplication>
#include <QHBoxLayout>

LegacyTrayApplet::LegacyTrayApplet()
: m_selectionManager(new SystemTray::FdoSelectionManager)
{
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    connect(m_selectionManager, SIGNAL(taskCreated(SystemTray::Task*)),
        SLOT(slotTaskCreated(SystemTray::Task*)));
}

LegacyTrayApplet::~LegacyTrayApplet()
{
    delete m_selectionManager;
}

void LegacyTrayApplet::slotTaskCreated(SystemTray::Task* task)
{
    task->createWidget();
    connect(task, SIGNAL(widgetCreated(QWidget*)), SLOT(slotWidgetCreated(QWidget*)));
}

void LegacyTrayApplet::slotWidgetCreated(QWidget* widget)
{
    layout()->addWidget(widget);
}

#include "legacytrayapplet.moc"
