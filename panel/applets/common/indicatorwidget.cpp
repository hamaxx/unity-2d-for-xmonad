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
#include "indicatorwidget.h"

// Local
#include <debug_p.h>
#include <indicatorentrywidget.h>
#include <indicatorsmanager.h>

// Qt
#include <QHBoxLayout>

using namespace unity::indicator;

IndicatorWidget::IndicatorWidget(const Indicator::Ptr& indicator, IndicatorsManager* manager)
: m_layout(new QHBoxLayout(this))
, m_indicatorsManager(manager)
, m_indicator(indicator)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    m_indicator->on_entry_added.connect(sigc::mem_fun(this, &IndicatorWidget::onEntryAdded));
}

void IndicatorWidget::onEntryAdded(const Entry::Ptr& entry)
{
    IndicatorEntryWidget* widget = new IndicatorEntryWidget(entry);
    m_indicatorsManager->addIndicatorEntryWidget(widget);
    m_layout->addWidget(widget);
}


#include "indicatorwidget.moc"
