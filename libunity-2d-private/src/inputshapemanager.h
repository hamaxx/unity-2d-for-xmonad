/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Ugo Riboni <ugo.riboni@canonical.com>
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

#ifndef INPUTSHAPEMANAGER_H
#define INPUTSHAPEMANAGER_H

#include <QObject>
#include <QDeclarativeListProperty>

#include "inputshaperectangle.h"

class Unity2DDeclarativeView;

class InputShapeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Unity2DDeclarativeView* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QDeclarativeListProperty<InputShapeRectangle> shapes READ shapes)
    Q_CLASSINFO("DefaultProperty", "shapes")

public:
    explicit InputShapeManager(QObject *parent = 0);
    Unity2DDeclarativeView* target() const;
    void setTarget(Unity2DDeclarativeView* target);
    QDeclarativeListProperty<InputShapeRectangle> shapes();

Q_SIGNALS:
    void targetChanged();

public Q_SLOTS:
    void updateManagedShape();

protected:
    static void appendShape(QDeclarativeListProperty<InputShapeRectangle> *property, InputShapeRectangle *value);

private:
    Unity2DDeclarativeView* m_target;
    QList<InputShapeRectangle*> m_shapes;
};

#endif // INPUTSHAPEMANAGER_H
