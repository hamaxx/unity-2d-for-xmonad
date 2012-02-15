/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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
#ifndef SHELLMANAGER_H
#define SHELLMANAGER_H

#include <QObject>
struct ShellManagerPrivate;

class QUrl;

class ShellManager : public QObject
{
    Q_OBJECT

    Q_ENUMS(DashMode)

    Q_PROPERTY(DashMode dashMode READ dashMode WRITE setDashMode NOTIFY dashModeChanged)

public:
    enum DashMode {
        DesktopMode,
        FullScreenMode
    };

    ShellManager(const QUrl &sourceFileUrl, QObject* parent = 0);
    ~ShellManager();

    DashMode dashMode() const;
    Q_INVOKABLE void setDashMode(DashMode);

Q_SIGNALS:
    void dashModeChanged(DashMode);

private Q_SLOTS:
    void onScreenCountChanged(int);
    void onAltF1Pressed();
    void onAltF2Pressed();
    void onNumericHotkeyPressed();

private:
    Q_DISABLE_COPY(ShellManager)
    ShellManagerPrivate * const d;
};

#endif // SHELLMANAGER_H
