/*
 * This file is part of unity-2d
 *
 * Copyright 2010, 2012 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Albert Astals Cid <albert.astals@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
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
#include "strutmanager.h"

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>

// X
#include <X11/Xlib.h>
#include <X11/Xatom.h>

static void setStrut(ulong* struts, WId effectiveWinId)
{
    static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
    XChangeProperty(QX11Info::display(), effectiveWinId, atom,
                    XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *) struts, 12);
}

StrutManager::StrutManager()
 : m_enabled(true),
   m_widget(NULL),
   m_edge(Unity2dPanel::TopEdge),
   m_width(-1),
   m_height(-1)
{
    QDesktopWidget* desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SLOT(updateStrut()));
    connect(desktop, SIGNAL(workAreaResized(int)), SLOT(updateStrut()));
}

StrutManager::~StrutManager()
{
    if (m_enabled) {
        releaseStrut();
    }
}

bool StrutManager::enabled() const
{
    return m_enabled;
}

void StrutManager::setEnabled(bool value)
{
    if (m_enabled != value) {
        if (value) {
            reserveStrut();
        } else {
            releaseStrut();
        }
        m_enabled = value;
        Q_EMIT enabledChanged(value);
    }
}

QObject *StrutManager::widget() const
{
    return m_widget;
}

void StrutManager::setWidget(QObject *widget)
{
    if (m_widget != widget) {
        if (m_widget) {
            m_widget->removeEventFilter(this);
            releaseStrut();
        }
        m_widget = qobject_cast<QWidget*>(widget);
        Q_ASSERT(m_widget != NULL);
        m_widget->installEventFilter(this);
        updateStrut();
        Q_EMIT widgetChanged(m_widget);
    }
}

Unity2dPanel::Edge StrutManager::edge() const
{
    return m_edge;
}

void StrutManager::setEdge(Unity2dPanel::Edge edge)
{
    if (m_edge != edge) {
        m_edge = edge;
        updateStrut();
        Q_EMIT edgeChanged(m_edge);
    }
}

int StrutManager::width() const
{
    return m_width;
}

void StrutManager::setWidth(int width)
{
    if (m_width != width) {
        m_width = width;
        updateStrut();
        Q_EMIT widthChanged(m_width);
    }
}

int StrutManager::realWidth() const
{
    if (m_widget == NULL)
        return m_width;

    if (m_width == -1)
        return m_widget->width();

    return m_width;
}

int StrutManager::height() const
{
    return m_height;
}

void StrutManager::setHeight(int height)
{
    if (m_height != height) {
        m_height = height;
        updateStrut();
        Q_EMIT heightChanged(m_height);
    }
}

int StrutManager::realHeight() const
{
    if (m_widget == NULL)
        return m_height;

    if (m_height == -1)
        return m_widget->height();

    return m_height;
}

void StrutManager::updateStrut()
{
    if (m_enabled) {
        reserveStrut();
    }
}

void StrutManager::reserveStrut()
{
    if (m_widget == NULL)
        return;

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect screen = desktop->screenGeometry(m_widget);
    const QRect available = desktop->availableGeometry(m_widget);

    ulong struts[12] = {};
    switch (m_edge) {
    case Unity2dPanel::LeftEdge:
        if (QApplication::isLeftToRight()) {
            struts[0] = screen.x() + realWidth();
            struts[4] = available.top();
            struts[5] = available.y() + available.height();
        } else {
            // Find the right-most X
            int rightMostX = 0;
            for (int i = 0; i < desktop->screenCount(); ++i) {
              rightMostX = qMax(rightMostX, desktop->screenGeometry(i).right());
            }
            struts[1] = (rightMostX - screen.right()) + realWidth();
            struts[6] = available.top();
            struts[7] = available.y() + available.height();
        }
        break;
    case Unity2dPanel::TopEdge:
        struts[2] = screen.y() + realHeight();
        struts[8] = screen.left();
        struts[9] = screen.x() + screen.width();
        break;
    }

    setStrut(struts, m_widget->effectiveWinId());
}

void StrutManager::releaseStrut()
{
    if (m_widget == NULL)
        return;

    ulong struts[12];
    memset(struts, 0, sizeof struts);
    setStrut(struts, m_widget->effectiveWinId());
}

bool StrutManager::eventFilter(QObject *watched, QEvent *event)
{
    Q_ASSERT(watched == m_widget);

    if (event->type() == QEvent::Show || event->type() == QEvent::Resize) {
        updateStrut();
    }
    return QObject::eventFilter(watched, event);
}

#include "strutmanager.moc"
