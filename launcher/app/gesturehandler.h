/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#ifndef GESTUREHANDLER_H
#define GESTUREHANDLER_H

#include <QObject>
#include <QHash>

extern "C" {
  #include <geis/geis.h>
}

#include "unity2dpanel.h"

class GestureHandler : public QObject
{
    Q_OBJECT
public:
    explicit GestureHandler(Unity2dPanel* launcher, QObject *parent = 0);
    ~GestureHandler();

private Q_SLOTS:
    void geisEventDispatch();

private:
    GeisStatus geisInitialize();
    GeisStatus geisStartEventDispatching();
    GeisStatus geisSubscribeGestures();

    static QHash<QString, GeisGestureAttr> parseGestureAttributes(GeisSize attr_count, GeisGestureAttr *attrs);
    static void staticGestureStart(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                   GeisSize attr_count, GeisGestureAttr *attrs);
    static void staticGestureUpdate(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                    GeisSize attr_count, GeisGestureAttr *attrs);
    static void staticGestureFinish(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                    GeisSize attr_count, GeisGestureAttr *attrs);
    void gestureStart(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);
    void gestureUpdate(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);
    void gestureFinish(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);

    GeisInstance m_geisInstance;
    GeisGestureFuncs m_gestureFuncs;
    float m_pinchPreviousRadius;
    int m_pinchPreviousTimestamp;
    float m_dragDelta;
    Unity2dPanel* m_launcher;
};

#endif // GESTUREHANDLER_H
