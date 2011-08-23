/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Alberto Mardegan <mardy@users.sourceforge.net>
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

#ifndef HOMEBUTTON_H
#define HOMEBUTTON_H

// Qt
#include <QTime>
#include <QToolButton>

class HomeButton : public QToolButton
{
Q_OBJECT
public:
    HomeButton(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent*);

private:
    Q_DISABLE_COPY(HomeButton)
    QTime lastClickTime;
};

#endif /* HOMEBUTTON_H */
