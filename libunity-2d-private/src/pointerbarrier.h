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

//Qt
#include <QLine>
#include <QTimer>

// X11
#include <X11/extensions/Xfixes.h>

struct PointerBarrierWrapperPrivate;

class PointerBarrierWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
    
friend class PointerBarrierManager;

public:
    PointerBarrierWrapper(QObject* parent = 0);
    ~PointerBarrierWrapper();

    QPointF p1() const;
    void setP1(const QPointF &p);
    
    QPointF p2() const;
    void setP2(const QPointF &p);

    PointerBarrier barrier() const;

Q_SIGNALS:
    void p1Changed(const QPointF &p1);
    void p2Changed(const QPointF &p2);
    
    void barrierHit(int x, int y, int velocity, int eventId);

private Q_SLOTS:
    void smoother();
    
    void updateEdgeStopVelocity();

private:
    void createBarrier();
    void destroyBarrier();
    
    void doProcess(XFixesBarrierNotifyEvent *event);

    PointerBarrier m_barrier;

    QPointF m_p1;
    QPointF m_p2;
    int m_maxVelocityMultiplier;
    int m_smoothing;
    QTimer *m_smoothingTimer;
    int m_pressure;

    int m_lastEventId;
    int m_lastX;
    int m_lastY;
    int m_smoothingCount;
    int m_smoothingAccumulator;
};

#endif // POINTERBARRIER_H
