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

#ifndef POINTERBARRIER_H
#define POINTERBARRIER_H

#include <QObject>
#include <QPointF>

// X11
#include <X11/extensions/Xfixes.h>

struct PointerBarrierWrapperPrivate;

class PointerBarrierWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(int maxVelocityMultiplier READ maxVelocityMultiplier WRITE setMaxVelocityMultiplier NOTIFY maxVelocityMultiplierChanged)
    Q_PROPERTY(int decayRate READ decayRate WRITE setDecayRate NOTIFY decayRateChanged)
    Q_PROPERTY(int breakPressure READ breakPressure WRITE setBreakPressure NOTIFY breakPressureChanged)

friend class PointerBarrierManager;

public:
    PointerBarrierWrapper(QObject* parent = 0);
    ~PointerBarrierWrapper();

    QPointF p1() const;
    void setP1(const QPointF &p);

    QPointF p2() const;
    void setP2(const QPointF &p);

    bool enabled() const;
    void setEnabled(bool enabled);

    int threshold() const;
    void setThreshold(int threshold);

    qreal maxVelocityMultiplier() const;
    void setMaxVelocityMultiplier(qreal maxVelocityMultiplier);

    int decayRate() const;
    void setDecayRate(int decayRate);

    int breakPressure() const;
    void setBreakPressure(int breakPressure);

    PointerBarrier barrier() const;

Q_SIGNALS:
    void p1Changed(const QPointF &p1);
    void p2Changed(const QPointF &p2);
    void enabledChanged(bool changed);
    void thresholdChanged(int threshold);
    void maxVelocityMultiplierChanged(qreal maxVelocityMultiplier);
    void decayRateChanged(int decayRate);
    void breakPressureChanged(int breakPressure);

    void barrierBroken();

private Q_SLOTS:
    void smoother();
    void decay();

private:
    void createBarrier();
    void destroyBarrier();

    void doProcess(XFixesBarrierNotifyEvent *event);

    void updateRealDecayTargetPressure();

    PointerBarrier m_barrier;

    QPointF m_p1;
    QPointF m_p2;
    bool m_enabled;
    int m_threshold;
    qreal m_maxVelocityMultiplier;
    int m_decayRate;
    int m_breakPressure;

    QTimer *m_smoothingTimer;

    int m_lastEventId;
    int m_lastX;
    int m_lastY;
    int m_smoothingCount;
    int m_smoothingAccumulator;

    int m_value;
    int m_targetPressure;
    int m_realDecayRate;
    QTimer *m_valueDecayTimer;
};

#endif // POINTERBARRIER_H
