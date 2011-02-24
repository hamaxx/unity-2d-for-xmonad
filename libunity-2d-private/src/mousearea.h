/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MOUSEAREA_H
#define MOUSEAREA_H

// Local
#include <unity2dapplication.h>

// Qt
#include <QObject>
#include <QRect>

struct MouseAreaPrivate;
/**
 * A class to help detecting when the mouse cursor enters an area.
 * Quite similar to QML MouseArea item.
 */
class MouseArea : public QObject, protected AbstractX11EventFilter
{
Q_OBJECT
public:
    MouseArea(QObject* parent = 0);
    ~MouseArea();

    QRect geometry() const;
    void setGeometry(int x, int y, int width, int height);
    void setGeometry(const QRect&);

    bool containsMouse() const;

Q_SIGNALS:
    void entered();
    void exited();

protected:
    bool x11EventFilter(XEvent*);

private:
    MouseAreaPrivate* const d;
};

#endif /* MOUSEAREA_H */
