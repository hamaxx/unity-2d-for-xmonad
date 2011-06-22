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
// Self
#include "unitycoreapplet.h"

// Local
#include <debug_p.h>
#include <indicatorwidget.h>

// Qt
#include <QHBoxLayout>

using namespace unity::indicator;

UnityCoreApplet::UnityCoreApplet()
: m_indicators(new DBusIndicators())
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_indicators->on_object_added.connect(
        sigc::mem_fun(this, &UnityCoreApplet::onObjectAdded)
        );
}

void UnityCoreApplet::onObjectAdded(Indicator::Ptr const& indicator)
{
    QString name = QString::fromStdString(indicator->name());
    if (name == "libappmenu.so") {
        // Skip appmenu for now
        return;
    }
    IndicatorWidget* widget = new IndicatorWidget(indicator);
    layout()->addWidget(widget);
}

#include "unitycoreapplet.moc"
