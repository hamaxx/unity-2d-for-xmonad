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

// Local
#include <unity2dapplication.h>

// X11
#include <X11/extensions/Xfixes.h>

struct PointerBarrierWrapperPrivate;

class PointerBarrierWrapper : public QObject, protected AbstractX11EventFilter
{
    Q_OBJECT

public:
    PointerBarrierWrapper(const QLine&, const int, QObject* parent = 0);
    ~PointerBarrierWrapper();

    void createBarrier(const QLine&, const int threshold);
    void updateBarrier(const QLine&, const int threshold);
    void destroyBarrier();

Q_SIGNALS:
    void barrierHit(int x, int y, int velocity, int eventId);

protected:
    bool x11EventFilter(XEvent*);

private Q_SLOTS:
    void smoother();

private:
    PointerBarrierWrapperPrivate* d;
    PointerBarrier m_barrier;

    bool m_active;
    int m_eventBase;
    int m_errorBase;
    int m_maxVelocityMultiplier;
    int m_smoothing;
    QTimer *m_smoothingTimer;
    int m_threshold;
    int m_pressure;
};

#endif // POINTERBARRIER_H
