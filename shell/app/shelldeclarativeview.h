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

// libunity-2d-private
#include "unity2ddeclarativeview.h"
#include "unity2dapplication.h"

// Qt
#include <QTimer>

class LauncherClient;
class DashDBus;
class ScreenInfo;
class Hotkey;

class ShellDeclarativeView : public Unity2DDeclarativeView, public AbstractX11EventFilter
{
    Q_OBJECT

    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden to add notify
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)
    Q_PROPERTY(bool isTopLeftShell READ isTopLeftShell WRITE setIsTopLeftShell NOTIFY isTopLeftShellChanged)
    Q_PROPERTY(bool haveCustomHomeShortcuts READ haveCustomHomeShortcuts)

    /* These two properties and mouse movement tracking on the widget are added here only because
       we need to detect when the mouse is inside the area occupied by the lancher. This should
       be entirely doable in QML with MouseAreas, but the current implementation has serious
       issues with event bubbling so we can't use it.
       See http://bugreports.qt.nokia.com/browse/QTBUG-13007?focusedCommentId=137123 */
    Q_PROPERTY(QRect monitoredArea READ monitoredArea WRITE setMonitoredArea
                                   NOTIFY monitoredAreaChanged)
    Q_PROPERTY(bool monitoredAreaContainsMouse READ monitoredAreaContainsMouse
                                               NOTIFY monitoredAreaContainsMouseChanged)

public:
    explicit ShellDeclarativeView(const QUrl &sourceFileUrl = QUrl(), bool isTopLeftShell = false, int screen = 0);

    /* getters */
    bool haveCustomHomeShortcuts() const;
    bool expanded() const;
    bool superKeyHeld() const { return m_superKeyHeld; }
    QRect monitoredArea() const;
    bool monitoredAreaContainsMouse() const;
    bool isTopLeftShell() const { return m_isTopLeftShell; }

    /* setters */
    Q_INVOKABLE void setExpanded(bool);
    void setScreenNumber(int);
    int screenNumber() const;
    void setMonitoredArea(QRect monitoredArea);
    void setIsTopLeftShell(bool);

    virtual bool x11EventFilter(XEvent* event);

    void toggleLauncher();

    void processNumericHotkey(Hotkey*);

Q_SIGNALS:
    void expandedChanged(bool);
    void focusChanged();
    void monitoredAreaChanged();
    void monitoredAreaContainsMouseChanged();

    void addWebFavoriteRequested(const QUrl& url);
    void superKeyHeldChanged(bool superKeyHeld);
    void superKeyTapped();
    void activateShortcutPressed(int itemIndex);
    void newInstanceShortcutPressed(int itemIndex);
    void launcherFocusRequested();
    void isTopLeftShellChanged(bool);

private Q_SLOTS:
    void updateSuperKeyMonitoring();
    void updateSuperKeyHoldState();
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void ignoreSuperPress();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void updateShellPosition();

private:
    void focusOutEvent(QFocusEvent* event);
    void focusInEvent(QFocusEvent* event);
    void setWMFlags();
    void updateInputShape();

    bool m_expanded;

    bool m_superKeyPressed;
    bool m_superKeyHeld;
    bool m_superPressIgnored;
    QTimer m_superKeyHoldTimer;
    QRect m_monitoredArea;
    bool m_monitoredAreaContainsMouse;
    bool m_isTopLeftShell;
    QUrl m_sourceFileUrl;

    friend class LauncherDBus;
    friend class ShellManager;
};

Q_DECLARE_METATYPE(ShellDeclarativeView*)

#endif // ShellDeclarativeView_H
