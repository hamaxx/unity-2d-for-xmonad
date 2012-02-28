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
#include <QDebug>
#include <QX11Info>

// libunity-2d
#include "pointerbarriermanager.h"
#include "config.h"

// Self
#include "pointerbarrier.h"

PointerBarrierWrapper::PointerBarrierWrapper(QObject *parent)
    : QObject(parent)
    , m_barrier(0)
    , m_maxVelocityMultiplier(30) //??
    , m_smoothing(75)
    , m_smoothingTimer(new QTimer(this))
    , m_lastEventId(0)
    , m_lastX(0)
    , m_lastY(0)
    , m_smoothingCount(0)
    , m_smoothingAccumulator(0)
{
    m_smoothingTimer->setSingleShot(true);
    m_smoothingTimer->setInterval(m_smoothing);
    connect(m_smoothingTimer, SIGNAL(timeout()), this, SLOT(smoother()));
    
    connect(&launcher2dConfiguration(), SIGNAL(edgeStopVelocityChanged(int)), SLOT(updateEdgeStopVelocity()));
    
    PointerBarrierManager::instance()->addBarrier(this);
}

PointerBarrierWrapper::~PointerBarrierWrapper()
{
    PointerBarrierManager::instance()->removeBarrier(this);
    destroyBarrier();
}

QPointF
PointerBarrierWrapper::p1() const
{
    return m_p1;
}

void
PointerBarrierWrapper::setP1(const QPointF& p)
{
    if (p == m_p1) {
        return;
    }

    if (m_barrier != 0) {
        destroyBarrier();
    }

    m_p1 = p;
    Q_EMIT p1Changed(p);

    createBarrier();
}

QPointF
PointerBarrierWrapper::p2() const
{
    return m_p2;
}

void
PointerBarrierWrapper::setP2(const QPointF& p)
{
    if (p == m_p2) {
        return;
    }

    if (m_barrier != 0) {
        destroyBarrier();
    }

    m_p2 = p;
    Q_EMIT p2Changed(p);

    createBarrier();
}

void
PointerBarrierWrapper::createBarrier()
{
    QVariant value = launcher2dConfiguration().property("edgeStopVelocity");
    const int threshold = value.isValid() ? value.toInt() : -1;
    if (threshold < 0) {
        qDebug() << "Cannot create border with negative stop velocity";
        return;
    }

    if ((m_p1.x() != m_p2.x()) && (m_p1.y() != m_p2.y()) && m_p1 != m_p2)
    {
        qWarning() << "Barrier line must be horizontal or vertical only";
        return;
    }

    Display *display = QX11Info::display();

    m_barrier = XFixesCreatePointerBarrierVelocity(display,
                    DefaultRootWindow(display),
                    m_p1.x(), m_p1.y(),
                    m_p2.x(), m_p2.y(),
                    0,
                    threshold,
                    0,
                    NULL);
    Q_ASSERT(m_barrier != 0);
}

void
PointerBarrierWrapper::destroyBarrier()
{
    if (m_barrier != 0) {
        XFixesDestroyPointerBarrier(QX11Info::display(), m_barrier);
        m_barrier = 0;
    }
}

void
PointerBarrierWrapper::doProcess(XFixesBarrierNotifyEvent *notifyEvent)
{
    m_lastX = notifyEvent->x;
    m_lastY = notifyEvent->y;
    m_lastEventId = notifyEvent->event_id;
    m_smoothingAccumulator += notifyEvent->velocity;
    m_smoothingCount++;

    /* Gathers events for 'm_smoothing' miliseconds, then takes average */
    if (!m_smoothingTimer->isActive()) {
        m_smoothingTimer->start();
    }
}

PointerBarrier
PointerBarrierWrapper::barrier() const
{
    return m_barrier;
}

// TODO Release barrier

void
PointerBarrierWrapper::smoother()
{
    if (m_smoothingCount <= 0) {
        return;
    }
    int velocity = qMin(600 * m_maxVelocityMultiplier, m_smoothingAccumulator / m_smoothingCount);

    Q_EMIT barrierHit(m_lastX, m_lastY, velocity, m_lastEventId);

    m_smoothingAccumulator = 0;
    m_smoothingCount = 0;
}

void
PointerBarrierWrapper::updateEdgeStopVelocity()
{
    if (m_barrier != 0) {
        destroyBarrier();
        createBarrier();
    }
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
