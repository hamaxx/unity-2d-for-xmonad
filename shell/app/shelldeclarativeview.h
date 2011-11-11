/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef ShellDeclarativeView_H
#define ShellDeclarativeView_H

#include <unity2ddeclarativeview.h>

#include <QTimer>

class LauncherClient;
class ScreenInfo;
class DashDBus;

class ShellDeclarativeView : public Unity2DDeclarativeView
{
    Q_OBJECT
    Q_ENUMS(DashMode)

    Q_PROPERTY(bool dashActive READ dashActive WRITE setDashActive NOTIFY dashActiveChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(DashMode dashMode READ dashMode WRITE setDashMode NOTIFY dashModeChanged)
    Q_PROPERTY(QString activeLens READ activeLens WRITE setActiveLens NOTIFY activeLensChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden to add notify
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)

public:
    enum DashMode {
        DesktopMode,
        FullScreenMode
    };
    explicit ShellDeclarativeView();

    /* getters */
    bool dashActive() const;
    DashMode dashMode() const;
    const QString& activeLens() const;
    bool expanded() const;
    bool superKeyHeld() const { return m_superKeyHeld; }

    /* setters */
    Q_SLOT void setDashActive(bool active);
    Q_INVOKABLE void setDashMode(DashMode);
    Q_INVOKABLE void setActiveLens(const QString& activeLens);
    Q_INVOKABLE void setExpanded(bool);

Q_SIGNALS:
    void dashActiveChanged(bool);
    void dashModeChanged(DashMode);
    void expandedChanged(bool);
    void activeLensChanged(const QString&);
    void activateLens(const QString& lensId);
    void activateHome();
    void focusChanged();

    void addWebFavoriteRequested(const QUrl& url);
    void superKeyHeldChanged(bool superKeyHeld);
    void superKeyTapped();
    void activateShortcutPressed(int itemIndex);
    void newInstanceShortcutPressed(int itemIndex);

private Q_SLOTS:
    void updateSuperKeyMonitoring();
    void updateSuperKeyHoldState();
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void forwardNumericHotkey();
    void ignoreSuperPress();

    void toggleDash();
    void showCommandsLens();
    void onSuperSPressed();

protected:
    void resizeEvent(QResizeEvent*);
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void updateDashModeDependingOnScreenGeometry();

private:
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void updateMask();
    void setWMFlags();

    LauncherClient* m_launcherClient;
    DashMode m_mode;
    bool m_expanded;
    QString m_activeLens; /* Lens id of the active lens */
    bool m_active;

    QConf* m_dconf_launcher;
    bool m_superKeyPressed;
    bool m_superKeyHeld;
    bool m_superPressIgnored;
    QTimer m_superKeyHoldTimer;

    friend class DashDBus;
    friend class LauncherDBus;
};

Q_DECLARE_METATYPE(ShellDeclarativeView*)

#endif // ShellDeclarativeView_H
