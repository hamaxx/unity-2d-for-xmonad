/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
#ifndef VISIBILITYCONTROLLER_H
#define VISIBILITYCONTROLLER_H

// Local

// Qt
#include <QHash>
#include <QObject>
#include <QScopedPointer>

class QConf;

class AbstractVisibilityBehavior;
class Unity2dPanel;
class QDBusServiceWatcher;

/**
 * This class monitors the hide_mode dconf key and set up an instance of
 * AbstractVisibilityBehavior depending on its value
 *
 * It also tracks requests for forced visibility: the launcher or another
 * application (through launcher DBus API) can request the launcher to stay
 * visible for a while, for example because an application requests attention
 * or because the dash is visible. This is handled by the beginForceVisible()
 * and endForceVisible() methods.
 *
 * Internally it maintains a refcount-per-app of forced visibility requests so
 * that it can restore the default mode if an application quits without calling
 * endForceVisible().
 */
class VisibilityController : public QObject
{
Q_OBJECT
public:
    VisibilityController(Unity2dPanel* panel);
    ~VisibilityController();

    /**
     * Force visibility of the launcher.
     * service is the dbus service (@see QDBusConnection::baseService()) of the
     * application which requested forced visibility. It is set to an empty
     * string for internal requests.
     */
    Q_INVOKABLE void beginForceVisible(const QString& service = QString());
    Q_INVOKABLE void endForceVisible(const QString& service = QString());

private Q_SLOTS:
    void update();
    void slotServiceUnregistered(const QString&);
    void updatePanelVisibility(bool);

private:
    enum AutoHideMode {
        ManualHide,
        AutoHide,
        IntelliHide
    };
    Q_DISABLE_COPY(VisibilityController);
    Unity2dPanel* m_panel;
    QConf* m_dconf_launcher;
    QDBusServiceWatcher* m_dbusWatcher;
    QScopedPointer<AbstractVisibilityBehavior> m_behavior;

    typedef QHash<QString, int> ForceVisibleCountHash;
    ForceVisibleCountHash m_forceVisibleCountHash;

    void setBehavior(AbstractVisibilityBehavior*);
};

#endif /* VISIBILITYCONTROLLER_H */
