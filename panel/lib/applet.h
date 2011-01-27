/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef APPLET_H
#define APPLET_H

// Local

// Qt
#include <QWidget>

namespace Unity2d
{

struct AppletPrivate;
class Applet : public QWidget
{
Q_OBJECT
public:
    Applet();
    ~Applet();

private:
    Q_DISABLE_COPY(Applet)
    AppletPrivate* const d;
};

} // namespace

#endif /* APPLET_H */
