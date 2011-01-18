/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PANEL_H
#define PANEL_H

// Local

// Qt
#include <QWidget>

namespace Unity2d
{

struct PanelPrivate;
class Panel : public QWidget
{
Q_OBJECT
public:
    enum Edge {
        LeftEdge,
        TopEdge
    };

    Panel(QWidget* parent = 0);
    ~Panel();

    void setEdge(Edge);
    Edge edge() const;

    void addWidget(QWidget*);

    void addSpacer();

protected:
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    Q_DISABLE_COPY(Panel)
    PanelPrivate* const d;

protected Q_SLOTS:
    void workAreaResized(int screen);
};

} // namespace

#endif /* PANEL_H */
