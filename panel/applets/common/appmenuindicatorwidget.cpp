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
#include "appmenuindicatorwidget.h"

// Local
#include <indicatorentrywidget.h>

// Qt
#include <QHBoxLayout>

static const int MENU_ITEM_PADDING = 6;

using namespace unity::indicator;

AppMenuIndicatorWidget::AppMenuIndicatorWidget(const Indicator::Ptr& indicator)
: IndicatorWidget(indicator)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addStretch();
}

void AppMenuIndicatorWidget::addIndicatorEntryWidget(IndicatorEntryWidget* widget)
{
    widget->setPadding(MENU_ITEM_PADDING);
    // Insert *before* stretch
    m_layout->insertWidget(m_layout->count() - 1, widget);
}

#include "appmenuindicatorwidget.moc"
