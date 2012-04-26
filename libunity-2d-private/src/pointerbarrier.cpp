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
#include "config.h"
#include "debug_p.h"

// Self
#include "pointerbarrier.h"

PointerBarrierWrapper::PointerBarrierWrapper(QObject *parent)
    : QObject(parent)
    , m_barrier(0)
    , m_triggerDirection(TriggerFromAnywhere)
    , m_triggerZoneEnabled(false)
    , m_triggerOnly(false)
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

QPointF PointerBarrierWrapper::p1() const
{
    return m_p1;
}

void PointerBarrierWrapper::setP1(const QPointF& p)
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

QPointF PointerBarrierWrapper::p2() const
{
    return m_p2;
}

void PointerBarrierWrapper::setP2(const QPointF& p)
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

QPointF PointerBarrierWrapper::triggerZoneP1() const
{
    return m_triggerZoneP1;
}

void PointerBarrierWrapper::setTriggerZoneP1(const QPointF& p)
{
    if (p != m_triggerZoneP1) {
        m_triggerZoneP1 = p;
        Q_EMIT triggerZoneP1Changed(p);

        handleTriggerZoneChanged();
    }
}

QPointF PointerBarrierWrapper::triggerZoneP2() const
{
    return m_triggerZoneP2;
}

void PointerBarrierWrapper::setTriggerZoneP2(const QPointF& p)
{
    if (p != m_triggerZoneP2) {
        m_triggerZoneP2 = p;
        Q_EMIT triggerZoneP2Changed(p);

        handleTriggerZoneChanged();
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

bool PointerBarrierWrapper::triggerZoneEnabled() const
{
    return m_triggerZoneEnabled;
}

void PointerBarrierWrapper::setTriggerZoneEnabled(bool enabled)
{
    if (m_triggerZoneEnabled != enabled) {
        m_triggerZoneEnabled = enabled;
        Q_EMIT triggerZoneEnabledChanged(enabled);

        handleTriggerZoneChanged();
    }
}

bool PointerBarrierWrapper::triggerOnly() const
{
    return m_triggerOnly;
}

void PointerBarrierWrapper::setTriggerOnly(bool triggerOnly)
{
    if (triggerOnly != m_triggerOnly) {
        m_triggerOnly = triggerOnly;
        Q_EMIT triggerOnlyChanged(triggerOnly);
    }
}

void PointerBarrierWrapper::createBarrier()
{
    if (m_threshold < 0) {
        return;
    }

    if (!isPointAlignmentCorrect()) {
        return;
    }

    Display *display = QX11Info::display();

    m_barrier = XFixesCreatePointerBarrierVelocity(display,
                    DefaultRootWindow(display),
                    m_p1.x(), m_p1.y(),
                    m_p2.x(), m_p2.y(),
                    0,
                    m_threshold,
                    0,
                    NULL);
    if (m_barrier == 0) {
        UQ_WARNING << "Pointer barrier creation failed. This can happen when using a non standard X server, autohiding launcher and sticky edges won't be available.";
        launcher2dConfiguration().setProperty("hideMode", 0);
    }
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

    if (m_triggerOnly && !isLastEventAgainstTrigger()) {
        // We got to the barrier from the non triggering direction
        // Release it so the mouse can continue its travel
        Display *display = QX11Info::display();
        XFixesBarrierReleasePointer (display, m_barrier, m_lastEventId);
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

    if (isLastEventAgainstTrigger()) {
        if (m_triggerValue.addAndCheckExceedingTarget(velocity)) {
            Q_EMIT triggered();
        }
    } else {
        if (m_breakValue.addAndCheckExceedingTarget(velocity)) {
            Display *display = QX11Info::display();
            XFixesBarrierReleasePointer (display, m_barrier, m_lastEventId);

            Q_EMIT broken();
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

void PointerBarrierWrapper::handleTriggerZoneChanged()
{
    // Make sure barrier point alignment is still valid
    if (!isPointAlignmentCorrect()) {
        destroyBarrier();
    }
    // If there is no barrier try to create one now
    if (m_barrier == 0) {
        createBarrier();
    }
}

bool PointerBarrierWrapper::isPointAlignmentCorrect() const
{
    bool alignmentCorrect = false;

    // Outer points can't be the same
    if (m_p1 != m_p2) {
        // The two points need to be aligned either vertically or horizontally
        if (m_p1.x() == m_p2.x()) {
            alignmentCorrect = !m_triggerZoneEnabled || (m_triggerZoneP1.x() == m_p1.x() && m_triggerZoneP2.x() == m_p1.x());
        } else if (m_p1.y() == m_p2.y()) {
            alignmentCorrect = !m_triggerZoneEnabled || (m_triggerZoneP1.y() == m_p1.y() && m_triggerZoneP2.y() == m_p1.y());
        }
    }

    return alignmentCorrect;

}

bool PointerBarrierWrapper::isLastEventAgainstTrigger() const
{
    bool againstTrigger = false;
    if (m_triggerZoneEnabled && m_triggerZoneP1.x() == m_triggerZoneP2.x() && m_triggerZoneP1.y() <= m_lastEventY && m_triggerZoneP2.y() >= m_lastEventY) {
        againstTrigger = m_triggerDirection == TriggerFromAnywhere ||
                        (m_triggerDirection == TriggerFromRight && m_lastEventX >= m_triggerZoneP1.x()) ||
                        (m_triggerDirection == TriggerFromLeft && m_lastEventX < m_triggerZoneP1.x());
    }
    if (m_triggerZoneEnabled && m_triggerZoneP1.y() == m_triggerZoneP2.y() && m_triggerZoneP1.x() <= m_lastEventX && m_triggerZoneP2.x() >= m_lastEventX) {
        againstTrigger = m_triggerDirection == TriggerFromAnywhere ||
                        (m_triggerDirection == TriggerFromTop && m_lastEventY >= m_triggerZoneP1.y()) ||
                        (m_triggerDirection == TriggerFromBottom && m_lastEventY < m_triggerZoneP1.y());
    }
    return againstTrigger;
}

#include <pointerbarrier.moc>
