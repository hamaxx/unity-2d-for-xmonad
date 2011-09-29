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

#ifndef PanelManager_H
#define PanelManager_H

// QConf
#include <qconf.h>

// Qt
#include <QObject>
#include <QList>

class Unity2dPanel;

class PanelManager : public QObject
{
Q_OBJECT

public:
    PanelManager(QObject* parent = 0);
    ~PanelManager();

private:
    Q_DISABLE_COPY(PanelManager)
    QList<Unity2dPanel*> m_panels;
    QConf m_conf;

    Unity2dPanel* instantiatePanel(int screen);
    QStringList loadPanelConfiguration() const;

private Q_SLOTS:
    void onScreenCountChanged(int newCount);
    void onF10Pressed();
};

#endif // PanelManager_H

