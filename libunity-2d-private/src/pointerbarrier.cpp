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

// Qt
#include <QDebug>
#include <QTimer>
#include <QX11Info>

// libunity-2d
#include "pointerbarriermanager.h"

// Self
#include "pointerbarrier.h"

PointerBarrierWrapper::PointerBarrierWrapper(QObject *parent)
    : QObject(parent)
    , m_barrier(0)
    , m_enabled(false)
    , m_threshold(-1)
    , m_maxVelocityMultiplier(-1)
    , m_decayRate(-1)
    , m_breakPressure(-1)
    , m_smoothingTimer(new QTimer(this))
    , m_lastEventId(0)
    , m_lastX(0)
    , m_lastY(0)
    , m_smoothingCount(0)
    , m_smoothingAccumulator(0)
    , m_value(0)
    , m_valueDecayTimer(new QTimer(this))
{
    m_smoothingTimer->setSingleShot(true);
    m_smoothingTimer->setInterval(75);
    connect(m_smoothingTimer, SIGNAL(timeout()), this, SLOT(smoother()));

    m_valueDecayTimer->setInterval(10);
    connect(m_valueDecayTimer, SIGNAL(timeout()), this, SLOT(decay()));

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
    if (p != m_p1) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_p1 = p;
        Q_EMIT p1Changed(p);

        createBarrier();
    }
}

QPointF
PointerBarrierWrapper::p2() const
{
    return m_p2;
}

void
PointerBarrierWrapper::setP2(const QPointF& p)
{
    if (p != m_p2) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_p2 = p;
        Q_EMIT p2Changed(p);

        createBarrier();
    }
}

bool PointerBarrierWrapper::enabled() const
{
    return m_enabled;
}

void PointerBarrierWrapper::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged(enabled);

        if (enabled) {
            createBarrier();
        } else {
            destroyBarrier();
        }
    }
}

void
PointerBarrierWrapper::createBarrier()
{
    if (!m_enabled)
        return;

    if (m_threshold < 0)
        return;

    if (m_p1 == m_p2)
        return;

    if ((m_p1.x() != m_p2.x()) && (m_p1.y() != m_p2.y()))
        return;

    Display *display = QX11Info::display();

    m_barrier = XFixesCreatePointerBarrierVelocity(display,
                    DefaultRootWindow(display),
                    m_p1.x(), m_p1.y(),
                    m_p2.x(), m_p2.y(),
                    0,
                    m_threshold,
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

    /* Gathers events for m_smoothingTimer->interval() miliseconds, then takes average */
    if (!m_smoothingTimer->isActive()) {
        m_smoothingTimer->start();
    }
}

int
PointerBarrierWrapper::threshold() const
{
    return m_threshold;
}

void
PointerBarrierWrapper::setThreshold(int threshold)
{
    if (m_threshold != threshold) {
        m_threshold = threshold;
        destroyBarrier();
        createBarrier();
        Q_EMIT thresholdChanged(threshold);
    }
}

qreal
PointerBarrierWrapper::maxVelocityMultiplier() const
{
    return m_maxVelocityMultiplier;
}

void
PointerBarrierWrapper::setMaxVelocityMultiplier(qreal maxVelocityMultiplier)
{
    if (maxVelocityMultiplier != m_maxVelocityMultiplier) {
        m_maxVelocityMultiplier = maxVelocityMultiplier;
        Q_EMIT maxVelocityMultiplierChanged(maxVelocityMultiplier);

        updateRealDecayTargetPressure();
    }
}

int
PointerBarrierWrapper::decayRate() const
{
    return m_decayRate;
}

void
PointerBarrierWrapper::setDecayRate(int decayRate)
{
    if (decayRate != m_decayRate) {
        m_decayRate = decayRate;
        Q_EMIT decayRateChanged(decayRate);

        updateRealDecayTargetPressure();
    }
}

int
PointerBarrierWrapper::breakPressure() const
{
    return m_breakPressure;
}

void
PointerBarrierWrapper::setBreakPressure(int breakPressure)
{
    if (m_breakPressure != breakPressure) {
        m_breakPressure = breakPressure;
        Q_EMIT breakPressureChanged(breakPressure);

        updateRealDecayTargetPressure();
    }
}

PointerBarrier
PointerBarrierWrapper::barrier() const
{
    return m_barrier;
}

void
PointerBarrierWrapper::decay() {
  const int partial_decay = m_realDecayRate / 100;

  m_value -= partial_decay;

  if (m_value <= 0)
  {
    m_value = 0;
    m_valueDecayTimer->stop();
  }
}

void
PointerBarrierWrapper::smoother()
{
    if (m_maxVelocityMultiplier < 0 || m_decayRate < 0 || m_breakPressure < 0) {
        qWarning() << "PointerBarrierWrapper::smoother: maxVelocityMultiplier, decayRate or breakPressure not set";
        return;
    }

    if (m_smoothingCount <= 0) {
        return;
    }
    const int velocity = qMin<qreal>(600 * m_maxVelocityMultiplier, m_smoothingAccumulator / m_smoothingCount);

    m_value += velocity;
    if (!m_valueDecayTimer->isActive()) {
        m_valueDecayTimer->start();
    }
    if (m_value >= m_targetPressure)
    {
        Display *display = QX11Info::display();
        XFixesBarrierReleasePointer (display, m_barrier, m_lastEventId);
        m_value = 0;
        m_valueDecayTimer->stop();

        Q_EMIT barrierBroken();
    }

    m_smoothingAccumulator = 0;
    m_smoothingCount = 0;
}

void
PointerBarrierWrapper::updateRealDecayTargetPressure()
{
    // make the effect half as strong as specified as other values shouldn't scale
    // as quickly as the max velocity multiplier
    const float responsiveness_mult = ((m_maxVelocityMultiplier - 1) * .025) + 1;
    m_realDecayRate = m_decayRate * responsiveness_mult;
    m_targetPressure = m_breakPressure * responsiveness_mult;
}

#include <pointerbarrier.moc>
