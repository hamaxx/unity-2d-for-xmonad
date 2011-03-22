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
#ifndef ABSTRACTVISIBILITYBEHAVIOR_H
#define ABSTRACTVISIBILITYBEHAVIOR_H

// Local

// Qt
#include <QObject>

class Unity2dPanel;

/**
 * Base class for various visibility behaviors.
 *
 * Does not provide much for now, only a pointer to the panel the behavior
 * handles.
 */
class AbstractVisibilityBehavior : public QObject
{
    Q_OBJECT
public:
    AbstractVisibilityBehavior(Unity2dPanel* panel);
    ~AbstractVisibilityBehavior();

protected:
    Unity2dPanel* m_panel;
};

#endif /* ABSTRACTVISIBILITYBEHAVIOR_H */
