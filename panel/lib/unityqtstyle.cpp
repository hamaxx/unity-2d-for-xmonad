/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "unityqtstyle.h"

// Local

// uqpanel
#include <debug_p.h>

// Qt
#include <QGtkStyle>
#include <QPainter>
#include <QStyleOptionFrame>
#include <QWidget>

UnityQtStyle::UnityQtStyle()
: QProxyStyle(new QGtkStyle)
{
}

void UnityQtStyle::drawControl(QStyle::ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (element == QStyle::CE_MenuBarItem && widget) {
        // Avoid solid gray background behind the menubar items
        QProxyStyle::drawControl(element, option, painter, 0);
    } else if (element == QStyle::CE_MenuBarEmptyArea) {
        // Avoid gray borders around the menubar items
    } else {
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}
