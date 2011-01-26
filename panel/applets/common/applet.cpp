/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Self
#include "applet.h"

// Local
#include <panel.h>

// Qt
#include <QApplication>
#include <QPainter>

// System
#include <iostream>

namespace Unity2d
{

struct AppletPrivate
{
};

Applet::Applet()
: QWidget()
, d(new AppletPrivate)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

Applet::~Applet()
{
    delete d;
}

} // namespace

#include "applet.moc"
