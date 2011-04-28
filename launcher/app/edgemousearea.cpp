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
#include "edgemousearea.h"

// Local

// libunity-2d
#include <debug_p.h>

// Qt
#include <QDesktopWidget>
#include <QTimer>

EdgeMouseArea::EdgeMouseArea(QObject* parent)
: MouseArea(parent)
, m_updateTimer(new QTimer(this))
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
}

void EdgeMouseArea::updateGeometryFromScreen()
{
    int leftScreen = QApplication::desktop()->screenNumber(QPoint());
    QRect rect = QApplication::desktop()->screenGeometry(leftScreen);
    setGeometry(rect.left(), rect.top(), 1, rect.height());
}