/*
 * Copyright (C) 2012 Canonical, Ltd.
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

// Special ordering to bypass evil X11 include
#include <QDebug>

// Self
#include "pointerbarriermanager.h"

// libunity-2d
#include "pointerbarrier.h"

// Qt
#include <QX11Info>

// X
#include <X11/extensions/Xfixes.h>

PointerBarrierManager *PointerBarrierManager::instance()
{
    static PointerBarrierManager *bpm = NULL;
    if (bpm == NULL) bpm = new PointerBarrierManager();
    return bpm;
}

PointerBarrierManager::PointerBarrierManager()
{
    Display *display = QX11Info::display();

    XFixesQueryExtension(display, &m_eventBase, &m_errorBase);

    Unity2dApplication* application = Unity2dApplication::instance();
    if (application == NULL) {
        /* This can happen for example when using qmlviewer to run the launcher */
        qWarning() << "The application is not an Unity2dApplication."
                      "Barriers will not be monitored.";
    } else {
        application->installX11EventFilter(this);
    }

    /* Enables barrier detection events - only call once!! */
    XFixesSelectBarrierInput(display, DefaultRootWindow(display), 0xdeadbeef);
}

void PointerBarrierManager::addBarrier(PointerBarrierWrapper *barrier)
{
    m_barriers += barrier;
}

void PointerBarrierManager::removeBarrier(PointerBarrierWrapper *barrier)
{
    m_barriers -= barrier;
}

bool PointerBarrierManager::x11EventFilter(XEvent* event)
{
    if (event->type - m_eventBase == XFixesBarrierNotify) {
        XFixesBarrierNotifyEvent *notifyEvent = (XFixesBarrierNotifyEvent *)event;

        if (notifyEvent->subtype == XFixesBarrierHitNotify) {
            Q_FOREACH (PointerBarrierWrapper *barrier, m_barriers) {
                if (barrier->barrier() == notifyEvent->barrier) {
                    barrier->doProcess(notifyEvent);
                    return true;
                }
            }
        }
    }
    return false;

}
