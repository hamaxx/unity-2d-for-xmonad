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
class Hotkey;
class ShellManager;

class ShellDeclarativeView : public Unity2DDeclarativeView, public AbstractX11EventFilter
{
    Q_OBJECT

    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden to add notify

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
    ShellDeclarativeView(ShellManager *manager, const QUrl &sourceFileUrl, int screen);

    /* getters */
    QRect monitoredArea() const;
    bool monitoredAreaContainsMouse() const;

    /* setters */
    void setMonitoredArea(QRect monitoredArea);

    virtual bool x11EventFilter(XEvent* event);

    void toggleLauncher();

    Q_INVOKABLE void forceActivateWindow();
    Q_INVOKABLE void forceDeactivateWindow();

Q_SIGNALS:
    void focusChanged();
    void monitoredAreaChanged();
    void monitoredAreaContainsMouseChanged();

    void activateShortcutPressed(int itemIndex);
    void newInstanceShortcutPressed(int itemIndex);
    void launcherFocusRequested();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);

private Q_SLOTS:
    void updateShellPosition();

private:
    void focusOutEvent(QFocusEvent* event);
    void focusInEvent(QFocusEvent* event);
    void setWMFlags();
    void updateInputShape();

    QRect m_monitoredArea;
    bool m_monitoredAreaContainsMouse;
    QUrl m_sourceFileUrl;
    ShellManager *m_manager;

    friend class ShellManager;
};

Q_DECLARE_METATYPE(ShellDeclarativeView*)

#endif // ShellDeclarativeView_H
