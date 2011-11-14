/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "edgehitdetector.h"

// Local

// libunity-2d
#include <debug_p.h>

// Qt
#include <QDesktopWidget>
#include <QTimer>

static const int EDGE_HIT_DELAY = 500;

EdgeHitDetector::EdgeHitDetector(QObject* parent)
: QObject(parent)
, m_mouseArea(new MouseArea(this))
, m_updateTimer(new QTimer(this))
, m_edgeHitTimer(new QTimer(this))
{
    updateGeometryFromScreen();

    // Use a timer to delay geometry updates because sometimes when
    // QDesktopWidget emits its signals, immediatly asking for screen geometry
    // yields wrong results.
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(1000);
    connect(m_updateTimer, SIGNAL(timeout()), SLOT(updateGeometryFromScreen()));

    QDesktopWidget* desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), m_updateTimer, SLOT(start()));
    connect(desktop, SIGNAL(screenCountChanged(int)), m_updateTimer, SLOT(start()));

    m_edgeHitTimer->setInterval(EDGE_HIT_DELAY);
    m_edgeHitTimer->setSingleShot(true);
    connect(m_edgeHitTimer, SIGNAL(timeout()), SIGNAL(edgeHit()));

    connect(m_mouseArea, SIGNAL(entered()), m_edgeHitTimer, SLOT(start()));
    connect(m_mouseArea, SIGNAL(exited()), m_edgeHitTimer, SLOT(stop()));
}

void EdgeHitDetector::updateGeometryFromScreen()
{
    QPoint p = QApplication::isLeftToRight() ?
        QPoint() :
        QPoint(QApplication::desktop()->width() - 1, 0);
    QRect rect = QApplication::desktop()->screenGeometry(p);
    if (QApplication::isLeftToRight()) {
        m_mouseArea->setGeometry(rect.left(), rect.top(), 1, rect.height());
    } else {
        m_mouseArea->setGeometry(rect.right() - 1, rect.top(), 1, rect.height());
    }
}

#include "edgehitdetector.moc"
