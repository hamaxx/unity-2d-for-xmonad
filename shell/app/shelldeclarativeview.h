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
class ShellDBus;
class ScreenInfo;
class HotModifier;

class ShellDeclarativeView : public Unity2DDeclarativeView, public AbstractX11EventFilter
{
    Q_OBJECT
    Q_ENUMS(DashMode)

    Q_PROPERTY(bool dashActive READ dashActive WRITE setDashActive NOTIFY dashActiveChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(DashMode dashMode READ dashMode WRITE setDashMode NOTIFY dashModeChanged)
    Q_PROPERTY(QString activeLens READ activeLens WRITE setActiveLens NOTIFY activeLensChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden to add notify
    Q_PROPERTY(bool dashAlwaysFullScreen READ dashAlwaysFullScreen NOTIFY dashAlwaysFullScreenChanged)
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)
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
    enum DashMode {
        DesktopMode,
        FullScreenMode
    };
    explicit ShellDeclarativeView();

    /* getters */
    bool dashActive() const;
    bool haveCustomHomeShortcuts() const;
    DashMode dashMode() const;
    const QString& activeLens() const;
    bool expanded() const;
    bool superKeyHeld() const;
    bool dashAlwaysFullScreen() const;
    QRect monitoredArea() const;
    bool monitoredAreaContainsMouse() const;

    /* setters */
    Q_SLOT void setDashActive(bool active);
    Q_INVOKABLE void setDashMode(DashMode);
    Q_INVOKABLE void setActiveLens(const QString& activeLens);
    Q_INVOKABLE void setExpanded(bool);
    void setMonitoredArea(QRect monitoredArea);

    virtual bool x11EventFilter(XEvent* event);

Q_SIGNALS:
    void dashActiveChanged(bool);
    void dashModeChanged(DashMode);
    void expandedChanged(bool);
    void activeLensChanged(const QString&);
    void activateLens(const QString& lensId);
    void activateHome();
    void toggleHud();
    void focusChanged();
    void monitoredAreaChanged();
    void monitoredAreaContainsMouseChanged();

    void dashAlwaysFullScreenChanged(bool dashAlwaysFullScreen);
    void superKeyHeldChanged(bool superKeyHeld);
    void activateShortcutPressed(int itemIndex);
    void newInstanceShortcutPressed(int itemIndex);
    void launcherFocusRequested();

private Q_SLOTS:
    void updateSuperKeyMonitoring();
    void forwardNumericHotkey();

    void toggleDash();
    void showCommandsLens();
    void onAltF1Pressed();

    void updateDashAlwaysFullScreen();

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

    DashMode m_mode;
    bool m_expanded;
    QString m_activeLens; /* Lens id of the active lens */
    bool m_active;

    bool m_dashAlwaysFullScreen;
    QRect m_monitoredArea;
    bool m_monitoredAreaContainsMouse;

    HotModifier* m_superHotModifier;
    HotModifier* m_altHotModifier;

    friend class DashDBus;
};

Q_DECLARE_METATYPE(ShellDeclarativeView*)

#endif // ShellDeclarativeView_H
