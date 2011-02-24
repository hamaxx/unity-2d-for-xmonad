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
#ifndef AUTOHIDECONTROLLER_H
#define AUTOHIDECONTROLLER_H

// Local

// Qt
#include <QObject>

class QTimer;
class MouseArea;
class Unity2dPanel;

/**
 * This class implements the classic autohide-on-timeout behavior
 */
class AutohideController : public QObject
{
Q_OBJECT
public:
    AutohideController(Unity2dPanel* panel);
    ~AutohideController();

protected:
    bool eventFilter(QObject*, QEvent*);

private:
    Unity2dPanel* m_panel;
    MouseArea* m_mouseArea;
    QTimer* m_autohideTimer;

    void updateFromPanelGeometry();
};

#endif /* AUTOHIDECONTROLLER_H */
