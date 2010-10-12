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
#include "abstractindicator.h"

// Local

// Qt
#include <QAction>

AbstractIndicator::AbstractIndicator(QObject* parent)
: QObject(parent)
, m_action(new QAction(this))
{
}

AbstractIndicator::~AbstractIndicator()
{
}

QAction* AbstractIndicator::action() const
{
    return m_action;
}

#include "abstractindicator.moc"