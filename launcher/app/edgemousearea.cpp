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

// Qt
#include <QDesktopWidget>

EdgeMouseArea::EdgeMouseArea(QObject* parent)
: MouseArea(parent)
, m_screen(0)
{
    updateGeometryFromScreen();
}

void EdgeMouseArea::setScreen(int screen)
{
    m_screen = 0;
    updateGeometryFromScreen();
}

void EdgeMouseArea::updateGeometryFromScreen()
{
    QRect rect = QApplication::desktop()->screenGeometry(m_screen);
    setGeometry(rect.left(), rect.top(), 1, rect.height());
}