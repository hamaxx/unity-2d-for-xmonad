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
#include <keyboardmodifiersmonitor.h>

// Qt
#include <QGtkStyle>
#include <QMenu>
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

int UnityQtStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    if (metric == QStyle::PM_MenuBarVMargin) {
        // Avoid one-pixel gap above menuitem
        return 0;
    } else {
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

QSize UnityQtStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const
{
    QSize size = QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
    if (type == QStyle::CT_MenuBarItem && widget) {
        // Avoid three-pixel gap below menuitem
        size.setHeight(widget->height());
    }
    return size;
}

int UnityQtStyle::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const
{
    if (hint == QStyle::SH_UnderlineShortcut) {
        // The shortcut of an opened menu can be triggered without holding Alt
        // down, so we always show the underline. For all other widgets we only
        // show the underlines if alt is down.
        // Note that this is a bit hackish: it only works reliably if the
        // widget repaints itself when alt is pressed or released. For now only
        // the MenuBarWidget from the AppNameApplets does this.
        if (qobject_cast<const QMenu*>(widget)) {
            return true;
        } else {
            return KeyboardModifiersMonitor::instance()->keyboardModifiers() == Qt::AltModifier;
        }
    } else {
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}
