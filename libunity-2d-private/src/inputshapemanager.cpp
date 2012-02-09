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

#include <QDebug>
#include <QRect>
#include <QX11Info>
#include <QRegion>

#include "inputshapemanager.h"
#include "unity2ddeclarativeview.h"
#include "desktopinfo.h"

// X11
#include <X11/Xlib.h>
#include <X11/Xregion.h>
#include <X11/extensions/shape.h>

InputShapeManager::InputShapeManager(QObject *parent) :
    QObject(parent),
    m_target(0)
{
}

void InputShapeManager::updateManagedShape()
{
    if (m_target == NULL || !m_target->isVisible()) {
        return;
    }

    QBitmap inputShape(m_target->width(), m_target->height());
    inputShape.fill(Qt::color0);
    QPainter painter(&inputShape);

    Q_FOREACH(InputShapeRectangle* shape, m_shapes) {
        if (shape->enabled()) {
            painter.drawPixmap(shape->rectangle().x(), shape->rectangle().y(),
                               shape->shape());
        }
    }

    XShapeCombineRegion(QX11Info::display(), m_target->effectiveWinId(),
                        DesktopInfo::instance()->isCompositingManagerRunning() ? ShapeInput : ShapeBounding,
                        0, 0, QRegion(inputShape).handle(), ShapeSet);
}

Unity2DDeclarativeView* InputShapeManager::target() const
{
    return m_target;
}

void InputShapeManager::setTarget(Unity2DDeclarativeView *target)
{
    if (m_target != target) {
        if (m_target != NULL) m_target->disconnect(this);

        //TODO: de-shape the current target view, if any, before shaping the new one. not used now
        //      as we never change the view anyway.

        m_target = target;
        if (m_target != NULL) {
            // due to the way xshape works we need to re-apply the shaping every time the target window
            // is mapped again.
            connect(m_target, SIGNAL(visibleChanged(bool)), SLOT(updateManagedShape()));
            connect(m_target, SIGNAL(sceneResized(QSize)), SLOT(updateManagedShape()));
        }
        Q_EMIT targetChanged();
        updateManagedShape();
    }
}

QDeclarativeListProperty<InputShapeRectangle> InputShapeManager::shapes()
{
    return QDeclarativeListProperty<InputShapeRectangle>(this, this, &InputShapeManager::appendShape);
}

void InputShapeManager::appendShape(QDeclarativeListProperty<InputShapeRectangle> *list, InputShapeRectangle *shape)
{
    InputShapeManager* instance = qobject_cast<InputShapeManager*>(list->object);
    if (instance != NULL) {
        instance->m_shapes.append(shape);
        instance->connect(shape, SIGNAL(shapeChanged()), SLOT(updateManagedShape()));
        instance->connect(shape, SIGNAL(enabledChanged()), SLOT(updateManagedShape()));
        instance->updateManagedShape();
    }
}

#include "inputshapemanager.moc"
