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

// Qt
#include <QObject>
#include <QList>
#include <QTimer>

class Unity2dPanel;

class PanelManager : public QObject
{
Q_OBJECT

public:
    PanelManager(QObject* parent = 0);
    ~PanelManager();

Q_SIGNALS:
    void altKeyHeldChanged(bool);

private:
    Q_DISABLE_COPY(PanelManager)
    QList<Unity2dPanel*> m_panels;

    Unity2dPanel* instantiatePanel(int screen);
    QStringList loadPanelConfiguration() const;

    void toggleHud();

private Q_SLOTS:
    void updateScreenLayout(int newCount);
    void onAltF10Pressed();
    void onScreenResized(int screen);
    void ignoreAltPress();
    void updateAltKeyHoldState();
    void onKeyboardModifiersChanged(Qt::KeyboardModifiers modifiers);

private:
    QTimer m_altKeyHoldTimer;
    bool m_altKeyPressed;
    bool m_altKeyHeld;
    bool m_altPressIgnored;
};

#endif // PanelManager_H

