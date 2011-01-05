/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#ifndef SPREADVIEW_H
#define SPREADVIEW_H

#include <QDeclarativeView>

class SpreadView : public QDeclarativeView
{
    Q_OBJECT

public slots:
    /* FIXME: copied from places/app/dashdeclarativeview.h */
    void fitToAvailableSpace(int screen);

    /* FIXME: copied from places/app/dashdeclarativeview.h */
    void forceActivateWindow();

public:
    explicit SpreadView();
};

#endif // SPREADVIEW_H

