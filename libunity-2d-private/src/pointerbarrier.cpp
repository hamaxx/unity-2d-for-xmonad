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
    , m_triggerDirection(TriggerFromAnywhere)
    , m_triggerEnabled(false)
    , m_threshold(-1)
    , m_maxVelocityMultiplier(-1)
    , m_decayRate(-1)
    , m_triggerPressure(-1)
    , m_breakPressure(-1)
    , m_smoothingTimer(new QTimer(this))
    , m_lastEventX(0)
    , m_lastEventY(0)
    , m_lastEventId(0)
    , m_smoothingCount(0)
    , m_smoothingAccumulator(0)
{
    m_smoothingTimer->setSingleShot(true);
    m_smoothingTimer->setInterval(75);
    connect(m_smoothingTimer, SIGNAL(timeout()), this, SLOT(smoother()));

    PointerBarrierManager::instance()->addBarrier(this);
}

PointerBarrierWrapper::~PointerBarrierWrapper()
{
    PointerBarrierManager::instance()->removeBarrier(this);
    destroyBarrier();
}

QPointF PointerBarrierWrapper::triggerP1() const
{
    return m_triggerP1;
}

void PointerBarrierWrapper::setTriggerP1(const QPointF& p)
{
    if (p != m_triggerP1) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_triggerP1 = p;
        Q_EMIT triggerP1Changed(p);

        createBarrier();
    }
}

QPointF PointerBarrierWrapper::triggerP2() const
{
    return m_triggerP2;
}

void PointerBarrierWrapper::setTriggerP2(const QPointF& p)
{
    if (p != m_triggerP2) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_triggerP2 = p;
        Q_EMIT triggerP2Changed(p);

        createBarrier();
    }
}

QPointF PointerBarrierWrapper::breakP1() const
{
    return m_breakP1;
}

void PointerBarrierWrapper::setBreakP1(const QPointF& p)
{
    if (p != m_breakP1) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_breakP1 = p;
        Q_EMIT breakP1Changed(p);

        createBarrier();
    }
}

QPointF PointerBarrierWrapper::breakP2() const
{
    return m_breakP2;
}

void PointerBarrierWrapper::setBreakP2(const QPointF& p)
{
    if (p != m_breakP2) {
        if (m_barrier != 0) {
            destroyBarrier();
        }

        m_breakP2 = p;
        Q_EMIT breakP2Changed(p);

        createBarrier();
    }
}

PointerBarrierWrapper::TriggerDirection PointerBarrierWrapper::triggerDirection() const
{
    return m_triggerDirection;
}

void PointerBarrierWrapper::setTriggerDirection(TriggerDirection direction)
{
    if (direction != m_triggerDirection) {
        m_triggerDirection = direction;
        Q_EMIT triggerDirectionChanged(direction);
    }
}

bool PointerBarrierWrapper::triggerEnabled() const
{
    return m_triggerEnabled;
}

void PointerBarrierWrapper::setTriggerEnabled(bool enabled)
{
    if (m_triggerEnabled != enabled) {
        QPointF p1Before, p2Before, p1After, p2After;
        calculateOuterPoints(&p1Before, &p2Before);

        m_triggerEnabled = enabled;
        Q_EMIT triggerEnabledChanged(enabled);

        calculateOuterPoints(&p1After, &p2After);
        if (m_barrier != 0 && (p1Before != p1After || p2Before != p2After)) {
            destroyBarrier();
            createBarrier();
        }
    }
}

void PointerBarrierWrapper::calculateOuterPoints(QPointF *p1, QPointF *p2)
{
    if (m_triggerEnabled) {
        // The four points need to be aligned either vertically or horizontally
        if (m_triggerP1.x() == m_triggerP2.x() && m_triggerP1.x() == m_breakP1.x() && m_triggerP1.x() == m_breakP2.x()) {
            p1->setX(m_triggerP1.x());
            p2->setX(m_triggerP1.x());
            p1->setY(qMin(m_triggerP1.y(), m_breakP1.y()));
            p2->setY(qMax(m_triggerP2.y(), m_breakP2.y()));
        } else if (m_triggerP1.y() == m_triggerP2.y() && m_triggerP1.y() == m_breakP1.y() && m_triggerP1.y() == m_breakP2.y()) {
            p1->setY(m_triggerP1.y());
            p2->setY(m_triggerP1.y());
            p1->setX(qMin(m_triggerP1.x(), m_breakP1.x()));
            p2->setX(qMax(m_triggerP2.x(), m_breakP2.x()));
        }
    } else {
        // The two points need to be aligned either vertically or horizontally
        if (m_breakP1.x() == m_breakP2.x() || m_breakP1.y() == m_breakP2.y()) {
            *p1 = m_breakP1;
            *p2 = m_breakP2;
        }
    }
}

void PointerBarrierWrapper::createBarrier()
{
    if (m_threshold < 0)
        return;

    QPointF p1, p2;
    calculateOuterPoints(&p1, &p2);

    if (p1 == p2)
        return;

    Display *display = QX11Info::display();

    m_barrier = XFixesCreatePointerBarrierVelocity(display,
                    DefaultRootWindow(display),
                    p1.x(), p1.y(),
                    p2.x(), p2.y(),
                    0,
                    m_threshold,
                    0,
                    NULL);
    Q_ASSERT(m_barrier != 0);
}

void PointerBarrierWrapper::destroyBarrier()
{
    if (m_barrier != 0) {
        XFixesDestroyPointerBarrier(QX11Info::display(), m_barrier);
        m_barrier = 0;
    }
}

void PointerBarrierWrapper::doProcess(XFixesBarrierNotifyEvent *notifyEvent)
{
    m_lastEventX = notifyEvent->x;
    m_lastEventY = notifyEvent->y;
    m_lastEventId = notifyEvent->event_id;
    m_smoothingAccumulator += notifyEvent->velocity;
    m_smoothingCount++;

    /* Gathers events for m_smoothingTimer->interval() miliseconds, then takes average */
    if (!m_smoothingTimer->isActive()) {
        m_smoothingTimer->start();
    }
}

int PointerBarrierWrapper::threshold() const
{
    return m_threshold;
}

void PointerBarrierWrapper::setThreshold(int threshold)
{
    if (m_threshold != threshold) {
        m_threshold = threshold;
        destroyBarrier();
        createBarrier();
        Q_EMIT thresholdChanged(threshold);
    }
}

qreal PointerBarrierWrapper::maxVelocityMultiplier() const
{
    return m_maxVelocityMultiplier;
}

void PointerBarrierWrapper::setMaxVelocityMultiplier(qreal maxVelocityMultiplier)
{
    if (maxVelocityMultiplier != m_maxVelocityMultiplier) {
        m_maxVelocityMultiplier = maxVelocityMultiplier;
        Q_EMIT maxVelocityMultiplierChanged(maxVelocityMultiplier);

        updateRealDecayTargetPressures();
    }
}

int PointerBarrierWrapper::decayRate() const
{
    return m_decayRate;
}

void PointerBarrierWrapper::setDecayRate(int decayRate)
{
    if (decayRate != m_decayRate) {
        m_decayRate = decayRate;
        Q_EMIT decayRateChanged(decayRate);

        updateRealDecayTargetPressures();
    }
}

int PointerBarrierWrapper::triggerPressure() const
{
    return m_triggerPressure;
}

void PointerBarrierWrapper::setTriggerPressure(int pressure)
{
    if (m_triggerPressure != pressure) {
        m_triggerPressure = pressure;
        Q_EMIT triggerPressureChanged(pressure);

        updateRealDecayTargetPressures();
    }
}

int PointerBarrierWrapper::breakPressure() const
{
    return m_breakPressure;
}

void PointerBarrierWrapper::setBreakPressure(int breakPressure)
{
    if (m_breakPressure != breakPressure) {
        m_breakPressure = breakPressure;
        Q_EMIT breakPressureChanged(breakPressure);

        updateRealDecayTargetPressures();
    }
}

PointerBarrier PointerBarrierWrapper::barrier() const
{
    return m_barrier;
}

void PointerBarrierWrapper::smoother()
{
    if (m_maxVelocityMultiplier < 0 || m_decayRate < 0 || m_breakPressure < 0) {
        qWarning() << "PointerBarrierWrapper::smoother: maxVelocityMultiplier, decayRate or breakPressure not set";
        return;
    }

    if (m_smoothingCount <= 0) {
        return;
    }
    const int velocity = qMin<qreal>(600 * m_maxVelocityMultiplier, m_smoothingAccumulator / m_smoothingCount);

    bool againstTrigger = false;
    if (m_triggerEnabled && m_triggerP1.x() == m_triggerP2.x() && m_triggerP1.y() <= m_lastEventY && m_triggerP2.y() >= m_lastEventY) {
        againstTrigger = m_triggerDirection == TriggerFromAnywhere ||
                        (m_triggerDirection == TriggerFromRight && m_lastEventX >= m_triggerP1.x()) ||
                        (m_triggerDirection == TriggerFromLeft && m_lastEventX < m_triggerP1.x());
    }
    if (m_triggerEnabled && m_triggerP1.y() == m_triggerP2.y() && m_triggerP1.x() <= m_lastEventX && m_triggerP2.x() >= m_lastEventX) {
        againstTrigger = m_triggerDirection == TriggerFromAnywhere ||
                        (m_triggerDirection == TriggerFromTop && m_lastEventY >= m_triggerP1.y()) ||
                        (m_triggerDirection == TriggerFromBottom && m_lastEventY < m_triggerP1.y());
    }
    if (againstTrigger) {
        if (m_triggerValue.add(velocity)) {
            Q_EMIT barrierTriggered();
        }
    } else {
        if (m_breakValue.add(velocity)) {
            Display *display = QX11Info::display();
            XFixesBarrierReleasePointer (display, m_barrier, m_lastEventId);

            Q_EMIT barrierBroken();
        }
    }

    m_smoothingAccumulator = 0;
    m_smoothingCount = 0;
}

void PointerBarrierWrapper::updateRealDecayTargetPressures()
{
    // make the effect half as strong as specified as other values shouldn't scale
    // as quickly as the max velocity multiplier
    const float responsiveness_mult = ((m_maxVelocityMultiplier - 1) * .025) + 1;
    const int realDecayRate = m_decayRate * responsiveness_mult;
    m_triggerValue.setDecayRate(realDecayRate);
    m_breakValue.setDecayRate(realDecayRate);
    m_triggerValue.setTarget(m_triggerPressure * responsiveness_mult);
    m_breakValue.setTarget(m_breakPressure * responsiveness_mult);
}

#include <pointerbarrier.moc>
