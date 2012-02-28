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

#ifndef POINTERBARRIERMANAGER_H
#define POINTERBARRIERMANAGER_H

#include "unity2dapplication.h"

class PointerBarrierWrapper;

class PointerBarrierManager : protected AbstractX11EventFilter
{
public:
    static PointerBarrierManager *instance();

    void addBarrier(PointerBarrierWrapper *barrier);
    void removeBarrier(PointerBarrierWrapper *barrier);

protected:
    bool x11EventFilter(XEvent* event);

private:
    Q_DISABLE_COPY(PointerBarrierManager);

    PointerBarrierManager();
    QSet<PointerBarrierWrapper*> m_barriers;
    int m_eventBase;
    int m_errorBase;
};

#endif // POINTERBARRIERMANAGER_H
