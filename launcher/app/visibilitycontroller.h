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
#include <QObject>
#include <QScopedPointer>

class GConfItemQmlWrapper;

class AbstractVisibilityBehavior;
class Unity2dPanel;

/**
 * This class monitors the hide_mode gconf key and set up an HideController
 * depending on its value
 */
class VisibilityController : public QObject
{
Q_OBJECT
public:
    VisibilityController(Unity2dPanel* panel);
    ~VisibilityController();

    Q_INVOKABLE void beginForceVisible();
    Q_INVOKABLE void endForceVisible();

private Q_SLOTS:
    void update();

private:
    enum AutoHideMode {
        ManualHide,
        AutoHide,
        IntelliHide
    };
    Q_DISABLE_COPY(VisibilityController);
    Unity2dPanel* m_panel;
    GConfItemQmlWrapper* m_hideModeKey;
    QScopedPointer<AbstractVisibilityBehavior> m_behavior;
    int m_forceVisibleCount;

    void setBehavior(AbstractVisibilityBehavior*);
};

#endif /* VISIBILITYCONTROLLER_H */
