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
#ifndef ABSTRACTHIDEBEHAVIOR_H
#define ABSTRACTHIDEBEHAVIOR_H

// Local

// Qt
#include <QObject>

class Unity2dPanel;

/**
 * Base class for autohide and intellihide behaviors.
 *
 * Provides common features such as showing the launcher when an item requests
 * attention.
 */
class AbstractHideBehavior : public QObject
{
    Q_OBJECT
public:
    AbstractHideBehavior(Unity2dPanel* panel);
    ~AbstractHideBehavior();

    virtual void setRequestAttention(bool) = 0;

protected:
    Unity2dPanel* m_panel;
};

#endif /* ABSTRACTHIDEBEHAVIOR_H */
