/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Ugo Riboni <ugo.riboni@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PANELAPPLET_H
#define PANELAPPLET_H

// Local
class Unity2dPanel;

// Qt
#include <QWidget>

namespace Unity2d
{

class PanelAppletPrivate;
class PanelApplet : public QWidget
{
    Q_OBJECT
public:
    explicit PanelApplet(Unity2dPanel* panel);

    Unity2dPanel* panel() const;

private:
    Q_DISABLE_COPY(PanelApplet)
    PanelAppletPrivate* d_ptr;
    Q_DECLARE_PRIVATE(PanelApplet)
};

} // namespace Unity2d

#endif // PANELAPPLET_H
