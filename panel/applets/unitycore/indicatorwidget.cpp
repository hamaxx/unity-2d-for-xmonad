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
#include <indicatorentrywidget.h>

// Qt
#include <QHBoxLayout>

using namespace unity::indicator;

IndicatorWidget::IndicatorWidget(const Indicator::Ptr& indicator)
: m_indicator(indicator)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_indicator->on_entry_added.connect(sigc::mem_fun(this, &IndicatorWidget::onEntryAdded));
}

void IndicatorWidget::onEntryAdded(const Entry::Ptr& entry)
{
    IndicatorEntryWidget* widget = new IndicatorEntryWidget(entry);
    layout()->addWidget(widget);
}


#include "indicatorwidget.moc"
