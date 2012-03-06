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
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
    Q_PROPERTY(QPointF triggerZoneP1 READ triggerZoneP1 WRITE setTriggerZoneP1 NOTIFY triggerZoneP1Changed)
    Q_PROPERTY(QPointF triggerZoneP2 READ triggerZoneP2 WRITE setTriggerZoneP2 NOTIFY triggerZoneP2Changed)
    Q_PROPERTY(TriggerDirection triggerDirection READ triggerDirection WRITE setTriggerDirection NOTIFY triggerDirectionChanged)
    Q_PROPERTY(bool triggerZoneEnabled READ triggerZoneEnabled WRITE setTriggerZoneEnabled NOTIFY triggerZoneEnabledChanged)
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

    QPointF p1() const;
    void setP1(const QPointF &p);

    QPointF p2() const;
    void setP2(const QPointF &p);

    QPointF triggerZoneP1() const;
    void setTriggerZoneP1(const QPointF &p);

    QPointF triggerZoneP2() const;
    void setTriggerZoneP2(const QPointF &p);

    TriggerDirection triggerDirection() const;
    void setTriggerDirection(TriggerDirection direction);

    bool triggerZoneEnabled() const;
    void setTriggerZoneEnabled(bool enabled);

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
    void p1Changed(const QPointF &p1);
    void p2Changed(const QPointF &p2);
    void triggerZoneP1Changed(const QPointF &p1);
    void triggerZoneP2Changed(const QPointF &p2);
    void triggerDirectionChanged(TriggerDirection direction);
    void triggerZoneEnabledChanged(bool changed);
    void thresholdChanged(int threshold);
    void maxVelocityMultiplierChanged(qreal maxVelocityMultiplier);
    void decayRateChanged(int decayRate);
    void triggerPressureChanged(int breakPressure);
    void breakPressureChanged(int breakPressure);

    void triggered();
    void broken();

private Q_SLOTS:
    void smoother();

private:
    Q_DISABLE_COPY(PointerBarrierWrapper);

    void createBarrier();
    void destroyBarrier();

    void doProcess(XFixesBarrierNotifyEvent *event);

    void updateRealDecayTargetPressures();

    void handleTriggerZoneChanged();

    bool isPointAlignmentCorrect() const;

    PointerBarrier m_barrier;

    QPointF m_p1;
    QPointF m_p2;
    QPointF m_triggerZoneP1;
    QPointF m_triggerZoneP2;
    TriggerDirection m_triggerDirection;
    bool m_triggerZoneEnabled;
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
