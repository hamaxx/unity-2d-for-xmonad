/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#ifndef LAUNCHERVIEW
#define LAUNCHERVIEW

#include <QDeclarativeView>
#include <QList>
#include <QUrl>
#include <QTimer>
#include <QVariant>

#include <unity2ddeclarativeview.h>

class DeclarativeDragDropEvent;
class LauncherDBus;
class QConf;

class LauncherView : public Unity2DDeclarativeView
{
    Q_OBJECT
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden

public:
    explicit LauncherView(QWidget* parent = NULL);
    ~LauncherView();

    bool superKeyHeld() const { return m_superKeyHeld; }

Q_SIGNALS:
    void activateShortcutPressed(int itemIndex);
    void newInstanceShortcutPressed(int itemIndex);
    void superKeyHeldChanged(bool superKeyHeld);
    void superKeyTapped();
    void addWebFavoriteRequested(const QUrl& url);
    void focusChanged(bool focus);

private Q_SLOTS:
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void forwardNumericHotkey();
    void ignoreSuperPress();
    void updateSuperKeyMonitoring();
    void updateSuperKeyHoldState();
    void toggleDash();
    void showCommandsLens();

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

private:
    QConf* m_dconf_launcher;
    bool m_superKeyPressed;
    bool m_superKeyHeld;
    bool m_superPressIgnored;
    QTimer m_superKeyHoldTimer;

    friend class LauncherDBus;
};

#endif // LAUNCHERVIEW

