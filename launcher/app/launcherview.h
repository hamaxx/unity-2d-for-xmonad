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

#include "gconfitem-qml-wrapper.h"

class DeclarativeDragDropEvent;
class LauncherDBus;

class LauncherView : public QDeclarativeView
{
    Q_OBJECT
    Q_PROPERTY(bool superKeyHeld READ superKeyHeld NOTIFY superKeyHeldChanged)

public:
    explicit LauncherView(QWidget* parent = NULL);
    Q_INVOKABLE QList<QVariant> getColorsFromIcon(QUrl source, QSize size) const;
    Q_INVOKABLE void onDragEnter(DeclarativeDragDropEvent* event);
    Q_INVOKABLE void onDrop(DeclarativeDragDropEvent* event);

    bool superKeyHeld() const { return m_superKeyHeld; }

Q_SIGNALS:
    void desktopFileDropped(QString path);
    void webpageUrlDropped(const QUrl& url);
    void keyboardShortcutPressed(int itemIndex);
    void superKeyHeldChanged(bool superKeyHeld);
    void superKeyTapped();
    void addWebFavoriteRequested(const QUrl& url);

private Q_SLOTS:
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void forwardHotkey();
    void updateSuperKeyMonitoring();
    void updateSuperKeyHoldState();
    void toggleDash();
    void changeKeyboardShortcutsState(bool enabled);

private:
    QList<QUrl> getEventUrls(DeclarativeDragDropEvent* event);

    GConfItemQmlWrapper m_enableSuperKey;
    bool m_superKeyPressed;
    bool m_superKeyHeld;
    QTimer m_superKeyHoldTimer;

    friend class LauncherDBus;
};

#endif // LAUNCHERVIEW

