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

class ShellDeclarativeView;

class QUrl;

class ShellManager : public QObject
{
    Q_OBJECT

    Q_ENUMS(DashMode)

    Q_PROPERTY(bool dashActive READ dashActive WRITE setDashActive NOTIFY dashActiveChanged)
    Q_PROPERTY(DashMode dashMode READ dashMode WRITE setDashMode NOTIFY dashModeChanged)
    Q_PROPERTY(QString dashActiveLens READ dashActiveLens WRITE setDashActiveLens NOTIFY dashActiveLensChanged)
    Q_PROPERTY(bool dashHaveCustomHomeShortcuts READ dashHaveCustomHomeShortcuts)
    Q_PROPERTY(QObject *dashShell READ dashShell NOTIFY dashShellChanged)
    Q_PROPERTY(int dashScreen READ dashScreen NOTIFY dashScreenChanged)
    Q_PROPERTY(bool dashAlwaysFullScreen READ dashAlwaysFullScreen NOTIFY dashAlwaysFullScreenChanged)
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)
    Q_PROPERTY(bool hudActive READ hudActive WRITE setHudActive NOTIFY hudActiveChanged)
    Q_PROPERTY(QObject *hudShell READ hudShell NOTIFY hudShellChanged)
    Q_PROPERTY(int hudScreen READ hudScreen NOTIFY hudScreenChanged)
    Q_PROPERTY(unsigned int lastFocusedWindow READ lastFocusedWindow NOTIFY lastFocusedWindowChanged)

public:
    enum DashMode {
        DesktopMode,
        FullScreenMode
    };

    ShellManager(const QUrl &sourceFileUrl, QObject* parent = 0);
    ~ShellManager();

    bool dashActive() const;
    Q_SLOT void setDashActive(bool active);

    DashMode dashMode() const;
    Q_INVOKABLE void setDashMode(DashMode);

    const QString& dashActiveLens() const;
    Q_INVOKABLE void setDashActiveLens(const QString& activeLens);

    bool dashHaveCustomHomeShortcuts() const;

    QObject *dashShell() const;

    int dashScreen() const;

    bool dashAlwaysFullScreen() const;

    bool superKeyHeld() const;

    bool hudActive() const;
    void setHudActive(bool active);

    QObject *hudShell() const;

    int hudScreen() const;

    unsigned int lastFocusedWindow() const;

    void forceActivateShell(ShellDeclarativeView *shell);
    void forceDeactivateShell(ShellDeclarativeView *shell);

Q_SIGNALS:
    void dashActiveChanged(bool);
    void dashModeChanged(DashMode);
    void dashActiveLensChanged(const QString&);
    void dashShellChanged(QObject *shell);
    void dashScreenChanged(int screen);
    void dashAlwaysFullScreenChanged(bool dashAlwaysFullScreen);
    void superKeyHeldChanged(bool superKeyHeld);
    void hudActiveChanged();
    void hudShellChanged(QObject *shell);
    void hudScreenChanged(int screen);
    void lastFocusedWindowChanged(unsigned int wid);

    void activeWorkspaceChanged();
    void dashActivateHome();
    void dashActivateLens(const QString& lensId);
    void toggleHud();

private Q_SLOTS:
    void onScreenCountChanged(int);

    void updateSuperKeyMonitoring();

    void onAltF1Pressed();
    void onAltF2Pressed();
    void onNumericHotkeyPressed();
    void toggleDashRequested();
    void toggleHudRequested();

    void updateDashAlwaysFullScreen();

    void onActiveWorkspaceChanged();

    void onHudActivationShortcutChanged();

private:
    Q_DISABLE_COPY(ShellManager)
    ShellManagerPrivate * const d;

friend class DashDBus;
friend struct ShellManagerPrivate;
};

#endif // SHELLMANAGER_H
