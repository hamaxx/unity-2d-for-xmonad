/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
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

class QWidget;

/**
 * Base class for various visibility behaviors.
 *
 */
class AbstractVisibilityBehavior : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(QWidget* panel READ panel WRITE setPanel NOTIFY panelChanged)

public:
    AbstractVisibilityBehavior(QWidget* panel=0);
    ~AbstractVisibilityBehavior();

    // getters
    bool visible() const;
    QWidget* panel() const;

    // setters
    void setPanel(QWidget* panel);

Q_SIGNALS:
    void visibleChanged(bool visible);
    void panelChanged(QWidget* panel);

protected:
    QWidget* m_panel;
    bool m_visible;
};

#endif /* ABSTRACTVISIBILITYBEHAVIOR_H */
