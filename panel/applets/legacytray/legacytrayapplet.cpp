/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "legacytrayapplet.h"

// Local
#include "fdoselectionmanager.h"
#include "fdotask.h"

// uqpanel
#include <debug_p.h>

// Qt
#include <QHBoxLayout>

LegacyTrayApplet::LegacyTrayApplet()
: m_selectionManager(new SystemTray::FdoSelectionManager)
{
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
    QWidget* widget = task->widget();
    layout()->addWidget(widget);
}

#include "legacytrayapplet.moc"
