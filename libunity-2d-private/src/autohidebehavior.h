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
#ifndef AUTOHIDEBEHAVIOR_H
#define AUTOHIDEBEHAVIOR_H

// Local
#include <abstractvisibilitybehavior.h>

// Qt
#include <QObject>

class EdgeHitDetector;
class QTimer;
class QWidget;

/**
 * This class implements the classic autohide-on-timeout behavior
 */
class AutoHideBehavior : public AbstractVisibilityBehavior
{
    Q_OBJECT

    Q_PROPERTY(QWidget* panel READ panel WRITE setPanel NOTIFY panelChanged)

public:
    AutoHideBehavior(QWidget* panel=0);
    ~AutoHideBehavior();

    // setters
    void setPanel(QWidget *panel);

Q_SIGNALS:
    void panelChanged(QWidget *panel);

protected:
    bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void hidePanel();
    void showPanel();

private:
    QTimer* m_autohideTimer;
    EdgeHitDetector* m_edgeHitDetector;

    void createEdgeHitDetector();
};

#endif /* AUTOHIDEBEHAVIOR_H */
