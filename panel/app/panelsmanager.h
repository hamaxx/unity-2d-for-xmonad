/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Olivier Tilloy <olivier.tilloy@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef PanelsManager_H
#define PanelsManager_H

// Qt
#include <QObject>
#include <QList>

class Unity2dPanel;

class PanelsManager : public QObject
{
Q_OBJECT

public:
    PanelsManager(QObject* parent = 0);
    ~PanelsManager();

private:
    Q_DISABLE_COPY(PanelsManager)
    QList<Unity2dPanel*> m_panels;

private Q_SLOTS:
    void onScreenCountChanged(int newCount);
};

#endif // PanelsManager_H

