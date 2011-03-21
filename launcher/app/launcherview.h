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

#include "gconfitem-qml-wrapper.h"

class DeclarativeDragDropEvent;
class LauncherDBus;
class VisibilityController;

class LauncherView : public QDeclarativeView
{
    Q_OBJECT
    Q_PROPERTY(bool superKeyPressed READ superKeyPressed
                                    NOTIFY superKeyPressedChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged) // overridden

public:
    explicit LauncherView();
    Q_INVOKABLE QList<QVariant> getColorsFromIcon(QUrl source, QSize size) const;
    Q_INVOKABLE void onDragEnter(DeclarativeDragDropEvent* event);
    Q_INVOKABLE void onDrop(DeclarativeDragDropEvent* event);

    bool superKeyPressed() const { return m_superKeyPressed; }

Q_SIGNALS:
    void desktopFileDropped(QString path);
    void webpageUrlDropped(const QUrl& url);
    void keyboardShortcutPressed(int itemIndex);
    void superKeyPressedChanged(bool superKeyPressed);
    void addWebFavoriteRequested(const QUrl& url);
    void focusChanged(bool focus);

private Q_SLOTS:
    void setHotkeysForModifiers(Qt::KeyboardModifiers modifiers);
    void forwardHotkey();
    void updateSuperKeyMonitoring();

public Q_SLOTS:
    void activateWindow();

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

private:
    QList<QUrl> getEventUrls(DeclarativeDragDropEvent* event);
    void changeKeyboardShortcutsState(bool enabled);

    GConfItemQmlWrapper m_enableSuperKey;
    bool m_superKeyPressed;

    friend class LauncherDBus;
};

#endif // LAUNCHERVIEW

