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

class GestureHandler : public QObject
{
    Q_OBJECT
public:
    explicit GestureHandler(QObject *parent = 0);
    ~GestureHandler();

    void gestureStart(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);
    void gestureUpdate(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);
    void gestureFinish(GeisGestureType type, GeisGestureId id, QHash<QString, GeisGestureAttr> attributes);

private Q_SLOTS:
    void geisEventDispatch();

private:
    GeisStatus geisInitialize();
    GeisStatus geisStartEventDispatching();
    GeisStatus geisSubscribeGestures();

    GeisInstance m_geisInstance;
    GeisGestureFuncs m_gestureFuncs;
};

#endif // GESTUREHANDLER_H
