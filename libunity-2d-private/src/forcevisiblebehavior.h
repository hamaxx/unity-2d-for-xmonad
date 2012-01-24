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
#ifndef FORCEVISIBLEBEHAVIOR_H
#define FORCEVISIBLEBEHAVIOR_H

// Local
#include <abstractvisibilitybehavior.h>

// Qt
#include <QObject>

/**
 * Behavior used when someone requested the launcher to be visible even if it
 * is supposed to be hidden
 */
class ForceVisibleBehavior : public AbstractVisibilityBehavior
{
    Q_OBJECT
public:
    ForceVisibleBehavior(QWidget* panel=0);
};

#endif /* FORCEVISIBLEBEHAVIOR_H */
