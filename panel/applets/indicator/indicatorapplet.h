/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Marco Trevisan (Treviño) <3v1n0@ubuntu.com>
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

#ifndef INDICATORAPPLET_H
#define INDICATORAPPLET_H

// Local
#include <panelapplet.h>

// libunity-core
#include <UnityCore/Indicator.h>
#include "indicatorswidget.h"

class IndicatorsManager;

class IndicatorApplet : public Unity2d::PanelApplet, public sigc::trackable
{
Q_OBJECT
public:
    IndicatorApplet(Unity2dPanel* panel);

    bool eventFilter(QObject*, QEvent*);

private:
    Q_DISABLE_COPY(IndicatorApplet)
    IndicatorsManager* m_indicatorsManager;
    IndicatorsWidget*  m_indicatorsWidget;
    void onObjectAdded(unity::indicator::Indicator::Ptr const&);
    void onObjectRemoved(unity::indicator::Indicator::Ptr const&);
};

#endif /* INDICATORAPPLET_H */
