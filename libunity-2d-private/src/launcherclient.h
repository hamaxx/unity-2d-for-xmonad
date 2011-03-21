/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
#ifndef LAUNCHERCLIENT_H
#define LAUNCHERCLIENT_H

// Local

// Qt
#include <QObject>

struct LauncherClientPrivate;
/**
 * Makes it easy for unity-2d components to communicate with unity-2d-launcher
 */
class LauncherClient : public QObject
{
    Q_OBJECT
public:
    // The amount of pixels used by the launcher on the left edge when it is
    // fully visible.
    static const int MaximumWidth;

    LauncherClient(QObject* parent = 0);
    ~LauncherClient();

    void beginForceVisible();
    void endForceVisible();

private:
    LauncherClientPrivate* const d;
};

#endif /* LAUNCHERCLIENT_H */
