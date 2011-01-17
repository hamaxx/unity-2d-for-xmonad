/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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

#ifndef ABSTRACTINDICATOR_H
#define ABSTRACTINDICATOR_H

// Local

// Qt
#include <QObject>

class QAction;

class AbstractIndicator : public QObject
{
    Q_OBJECT
public:
    AbstractIndicator(QObject* parent=0);
    ~AbstractIndicator();

    /**
     * Called when the indicator has been constructed and its owner is connected to signals.
     * It's the right place to emit actionAdded()
     */
    virtual void init();

Q_SIGNALS:
    void actionAdded(QAction*);
    void actionRemoved(QAction*);

private:
    Q_DISABLE_COPY(AbstractIndicator)
};

#endif /* ABSTRACTINDICATOR_H */
