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
#include <QUrl>
#include <QList>
#include <QDragEnterEvent>
#include <QTimer>
#include "gconfitem-qml-wrapper.h"

class QGraphicsObject;
class Unity2dPanel;

class LauncherView : public QDeclarativeView
{
    Q_OBJECT
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)

public:
    explicit LauncherView(Unity2dPanel* parentPanel = NULL);
    Q_INVOKABLE QList<QVariant> getColorsFromIcon(QUrl source, QSize size) const;

    bool superKeyHeld() const { return m_superKeyHeld; }

Q_SIGNALS:
    void desktopFileDropped(QString path);
    void webpageUrlDropped(const QUrl& url);
    void keyboardShortcutPressed(int itemIndex);
    void superKeyHeldChanged(bool superKeyHeld);
    void superKeyTapped();

private Q_SLOTS:
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void forwardHotkey();
    void updateSuperKeyMonitoring();
    void updateSuperKeyHoldState();
    void toggleDash();
    void togglePanel(bool visible);
    void changeKeyboardShortcutsState(bool enabled);

private:
    QList<QUrl> getEventUrls(QDropEvent*);

    /* Custom drag’n’drop handling */
    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dropEvent(QDropEvent*);

    QGraphicsObject* launcherItemAt(const QPoint&) const;
    void delegateDragEventHandlingToItem(QDropEvent*, QGraphicsObject*);
    bool acceptDndEvent(QDragEnterEvent*);

    /* The launcher item currently under the mouse cursor during a dnd event */
    QGraphicsObject* m_dndCurrentLauncherItem;
    /* Whether it accepted the event */
    bool m_dndCurrentLauncherItemAccepted;
    /* Whether the launcher itself handles the current dnd event */
    bool m_dndAccepted;

    GConfItemQmlWrapper m_enableSuperKey;
    bool m_superKeyPressed;
    bool m_superKeyHeld;
    QTimer m_superKeyHoldTimer;

    Unity2dPanel *m_parentPanel;
};

#endif // LAUNCHERVIEW

