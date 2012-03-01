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

#ifndef POINTERBARRIER_H
#define POINTERBARRIER_H

#include <QObject>
#include <QPointF>

// X11
#include <X11/extensions/Xfixes.h>

#include "decayedvalue.h"

struct PointerBarrierWrapperPrivate;

class PointerBarrierWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF triggerP1 READ triggerP1 WRITE setTriggerP1 NOTIFY triggerP1Changed)
    Q_PROPERTY(QPointF triggerP2 READ triggerP2 WRITE setTriggerP2 NOTIFY triggerP2Changed)
    Q_PROPERTY(QPointF breakP1 READ breakP1 WRITE setBreakP1 NOTIFY breakP1Changed)
    Q_PROPERTY(QPointF breakP2 READ breakP2 WRITE setBreakP2 NOTIFY breakP2Changed)
    Q_PROPERTY(TriggerDirection triggerDirection READ triggerDirection WRITE setTriggerDirection NOTIFY triggerDirectionChanged)
    Q_PROPERTY(bool triggerEnabled READ triggerEnabled WRITE setTriggerEnabled NOTIFY triggerEnabledChanged)
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(int maxVelocityMultiplier READ maxVelocityMultiplier WRITE setMaxVelocityMultiplier NOTIFY maxVelocityMultiplierChanged)
    Q_PROPERTY(int decayRate READ decayRate WRITE setDecayRate NOTIFY decayRateChanged)
    Q_PROPERTY(int triggerPressure READ triggerPressure WRITE setTriggerPressure NOTIFY triggerPressureChanged)
    Q_PROPERTY(int breakPressure READ breakPressure WRITE setBreakPressure NOTIFY breakPressureChanged)

friend class PointerBarrierManager;

public:
    enum TriggerDirection {
        TriggerFromAnywhere,
        TriggerFromRight,
        TriggerFromLeft,
        TriggerFromTop,
        TriggerFromBottom
    };
    Q_ENUMS(TriggerDirection)

    PointerBarrierWrapper(QObject* parent = 0);
    ~PointerBarrierWrapper();

    QPointF triggerP1() const;
    void setTriggerP1(const QPointF &p);

    QPointF triggerP2() const;
    void setTriggerP2(const QPointF &p);

    QPointF breakP1() const;
    void setBreakP1(const QPointF &p);

    QPointF breakP2() const;
    void setBreakP2(const QPointF &p);

    TriggerDirection triggerDirection() const;
    void setTriggerDirection(TriggerDirection direction);

    bool triggerEnabled() const;
    void setTriggerEnabled(bool enabled);

    int threshold() const;
    void setThreshold(int threshold);

    qreal maxVelocityMultiplier() const;
    void setMaxVelocityMultiplier(qreal maxVelocityMultiplier);

    int decayRate() const;
    void setDecayRate(int decayRate);

    int triggerPressure() const;
    void setTriggerPressure(int pressure);

    int breakPressure() const;
    void setBreakPressure(int pressure);

    PointerBarrier barrier() const;

Q_SIGNALS:
    void triggerP1Changed(const QPointF &p1);
    void triggerP2Changed(const QPointF &p2);
    void breakP1Changed(const QPointF &p1);
    void breakP2Changed(const QPointF &p2);
    void enabledChanged(bool changed);
    void triggerDirectionChanged(TriggerDirection direction);
    void triggerEnabledChanged(bool changed);
    void thresholdChanged(int threshold);
    void maxVelocityMultiplierChanged(qreal maxVelocityMultiplier);
    void decayRateChanged(int decayRate);
    void triggerPressureChanged(int breakPressure);
    void breakPressureChanged(int breakPressure);

    void barrierTriggered();
    void barrierBroken();

private Q_SLOTS:
    void smoother();

private:
    Q_DISABLE_COPY(PointerBarrierWrapper);

    void calculateOuterPoints(QPointF *p1, QPointF *p2);

    void createBarrier();
    void destroyBarrier();

    void doProcess(XFixesBarrierNotifyEvent *event);

    void updateRealDecayTargetPressures();

    PointerBarrier m_barrier;

    QPointF m_triggerP1;
    QPointF m_triggerP2;
    QPointF m_breakP1;
    QPointF m_breakP2;
    TriggerDirection m_triggerDirection;
    bool m_triggerEnabled;
    int m_threshold;
    qreal m_maxVelocityMultiplier;
    int m_decayRate;
    int m_triggerPressure;
    int m_breakPressure;

    QTimer *m_smoothingTimer;

    int m_lastEventX;
    int m_lastEventY;
    int m_lastEventId;
    int m_smoothingCount;
    int m_smoothingAccumulator;

    DecayedValue m_triggerValue;
    DecayedValue m_breakValue;
};

#endif // POINTERBARRIER_H
