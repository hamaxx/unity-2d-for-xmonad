/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Gerry Boland <gerry.boland@canonical.com>
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

// Qt
#include <QX11Info>
#include <QDebug>

// Self
#include "pointerbarrier.h"

struct PointerBarrierWrapperPrivate
{
    PointerBarrierWrapperPrivate()
    : m_lastEventId(0)
    , m_lastX(0)
    , m_lastY(0)
    , m_smoothingCount(0)
    , m_smoothingAccumulator(0)
    {}

    int m_lastEventId;
    int m_lastX;
    int m_lastY;
    int m_smoothingCount;
    int m_smoothingAccumulator;
};

PointerBarrierWrapper::PointerBarrierWrapper(const QLine& line, const int threshold, QObject *parent)
    : QObject(parent)
    , d(new PointerBarrierWrapperPrivate)
    , m_active(false)
    , m_eventBase(0)
    , m_errorBase(0)
    , m_maxVelocityMultiplier(30) //??
    , m_smoothing(75)
    , m_smoothingTimer(new QTimer(this))
    , m_threshold(threshold)
{
    Display *display = QX11Info::display();

    XFixesQueryExtension(display, &m_eventBase, &m_errorBase);

    int maj, min;
    XFixesQueryVersion(display, &maj, &min);
    if (maj < 6) {
        qDebug() << "XFixes version 6 or greater required for PointerBarrierVelocity";
        return;
    }

    createBarrier(line, m_threshold);

    /* Enables barrier detection events - only call once!! */
    XFixesSelectBarrierInput(display, DefaultRootWindow(display), 0xdeadbeef);

    m_smoothingTimer->setSingleShot(true);
    m_smoothingTimer->setInterval(m_smoothing);
    connect(m_smoothingTimer, SIGNAL(timeout()), this, SLOT(smoother()));

    Unity2dApplication* application = Unity2dApplication::instance();
    if (application == NULL) {
        /* This can happen for example when using qmlviewer to run the launcher */
        qDebug() << "The application is not an Unity2dApplication."
                      "Modifiers will not be monitored.";
    } else {
        application->installX11EventFilter(this);
    }
}

PointerBarrierWrapper::~PointerBarrierWrapper()
{
    destroyBarrier();

    Unity2dApplication* application = Unity2dApplication::instance();
    if (application != NULL) {
        application->removeX11EventFilter(this);
    }
}

void
PointerBarrierWrapper::createBarrier(const QLine& line, int threshold)
{
    if (m_active) {
        qDebug() << "Border already created";
        return;
    }

    if (threshold < 0) {
        qDebug() << "Cannot create border with negative stop velocity";
        return;
    }

    if (line.isNull() ||
           ((line.x1() != line.x2()) && (line.y1() != line.y2())))
    {
        qDebug() << "Barrier line must be horizontal or vertical only";
        return;
    }

    Display *display = QX11Info::display();
    m_threshold = threshold;

    m_barrier = XFixesCreatePointerBarrierVelocity(display,
                    DefaultRootWindow(display),
                    line.x1(), line.y1(),
                    line.x2(), line.y2(),
                    0,
                    m_threshold,
                    0,
                    NULL);
    m_active = true;
}

void
PointerBarrierWrapper::destroyBarrier()
{
    if (m_active) {
        XFixesDestroyPointerBarrier(QX11Info::display(), m_barrier);
        m_active = false;
    }
}

void
PointerBarrierWrapper::updateBarrier(const QLine& line, int threshold)
{
    destroyBarrier();
    createBarrier(line, threshold);
}

bool
PointerBarrierWrapper::x11EventFilter(XEvent *event)
{
    if (event->type - m_eventBase == XFixesBarrierNotify) {

        XFixesBarrierNotifyEvent *notifyEvent = (XFixesBarrierNotifyEvent *)event;

        if (notifyEvent->barrier == m_barrier && notifyEvent->subtype == XFixesBarrierHitNotify) {
            d->m_lastX = notifyEvent->x;
            d->m_lastY = notifyEvent->y;
            d->m_lastEventId = notifyEvent->event_id;
            d->m_smoothingAccumulator += notifyEvent->velocity;
            d->m_smoothingCount++;

            /* Gathers events for 'm_smoothing' miliseconds, then takes average */
            if (!m_smoothingTimer->isActive()) {
                m_smoothingTimer->start();
            }
        }
        return (notifyEvent->barrier == m_barrier);
    }
    return false;
}

void
PointerBarrierWrapper::smoother()
{
    if (d->m_smoothingCount <= 0) {
        return;
    }
    int velocity = qMin(600 * m_maxVelocityMultiplier, d->m_smoothingAccumulator / d->m_smoothingCount);

    Q_EMIT barrierHit(d->m_lastX, d->m_lastY,
                    velocity,
                    d->m_lastEventId);

    d->m_smoothingAccumulator = 0;
    d->m_smoothingCount = 0;
}


/*   // make the effect half as strong as specified as other values shouldn't scale
  // as quickly as the max velocity multiplier
  float responsiveness_mult = ((options->edge_responsiveness() - 1) * .025) + 1;

  decaymulator_->rate_of_decay = options->edge_decay_rate() * responsiveness_mult;
  _edge_overcome_pressure = options->edge_overcome_pressure() * responsiveness_mult;

  _pointer_barrier->threshold = options->edge_stop_velocity();
  _pointer_barrier->max_velocity_multiplier = options->edge_responsiveness();
  _pointer_barrier->DestroyBarrier();
  _pointer_barrier->ConstructBarrier();

  _hide_machine->reveal_pressure = options->edge_reveal_pressure() * responsiveness_mult;
  _hide_machine->edge_decay_rate = options->edge_decay_rate() * responsiveness_mult;
  */

#include <pointerbarrier.moc>
