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
#include "indicatorapplet.h"

// Local
#include "abstractindicator.h"
#include "datetimeindicator.h"
#include "debug_p.h"

// Qt
#include <QAction>
#include <QDBusConnection>
#include <QHBoxLayout>
#include <QMenu>

IndicatorApplet::IndicatorApplet()
{
    setupUi();
    loadIndicators();
}

void IndicatorApplet::setupUi()
{
    m_menuBar = new QMenuBar;
    m_menuBar->setNativeMenuBar(false);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_menuBar);
}

void IndicatorApplet::loadIndicators()
{
    // FIXME: Using Qt plugins
    QList<AbstractIndicator*> indicators = QList<AbstractIndicator*>()
        << new DateTimeIndicator(this)
        ;

    Q_FOREACH(AbstractIndicator* indicator, indicators) {
        connect(indicator, SIGNAL(actionAdded(QAction*)), SLOT(slotActionAdded(QAction*)));
        connect(indicator, SIGNAL(actionRemoved(QAction*)), SLOT(slotActionRemoved(QAction*)));
        indicator->init();
    }
}

void IndicatorApplet::slotActionAdded(QAction* action)
{
    UQ_VAR(action->text());
    m_menuBar->addAction(action);
}

void IndicatorApplet::slotActionRemoved(QAction* action)
{
    m_menuBar->removeAction(action);
}


#include "indicatorapplet.moc"
