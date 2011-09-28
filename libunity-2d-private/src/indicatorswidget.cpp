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
// Self
#include "indicatorswidget.h"

// Local
#include <debug_p.h>
#include <indicatorentrywidget.h>
#include <indicatorsmanager.h>

// Qt
#include <QHBoxLayout>

using namespace unity::indicator;

IndicatorsWidget::IndicatorsWidget(IndicatorsManager* manager)
: m_layout(new QHBoxLayout(this))
, m_indicatorsManager(manager)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
}

IndicatorsWidget::~IndicatorsWidget()
{
    /* Disconnect from all the indicators' signals 'on_entry_added' and 'on_entry_removed' */
    Q_FOREACH(QList<sigc::connection> connections, m_indicators_connections) {
        Q_FOREACH(sigc::connection connection, connections) {
            connection.disconnect();
        }
    }
}

void IndicatorsWidget::addIndicator(const unity::indicator::Indicator::Ptr& indicator)
{
    sigc::connection conn;

    conn = indicator->on_entry_added.connect(
                                sigc::mem_fun(this, &IndicatorsWidget::onEntryAdded)
                            );
    m_indicators_connections[indicator].append(conn);

    conn = indicator->on_entry_removed.connect(
                                  sigc::mem_fun(this, &IndicatorsWidget::onEntryRemoved)
                              );
    m_indicators_connections[indicator].append(conn);
}

void IndicatorsWidget::removeIndicator(const unity::indicator::Indicator::Ptr& indicator)
{
    Q_FOREACH(sigc::connection conn, m_indicators_connections[indicator])
    {
        conn.disconnect();
    }

    Q_FOREACH(Entry::Ptr entry, indicator->GetEntries())
    {
        onEntryRemoved (entry->id());
    }

    m_indicators_connections.remove(indicator);
}

void IndicatorsWidget::onEntryAdded(const Entry::Ptr& entry)
{
    int indicator_pos = 0;
    IndicatorEntryWidget* widget = new IndicatorEntryWidget(entry);

    if (entry->priority() > -1) {
        Q_FOREACH(IndicatorEntryWidget *w, m_entries)
        {
            if (w->entry()->priority() >= entry->priority())
                break;

            indicator_pos++;
        }
    }

    m_indicatorsManager->addIndicatorEntryWidget(widget);
    m_layout->insertWidget(indicator_pos, widget);
    m_entries.insert(indicator_pos, widget);
}

void IndicatorsWidget::onEntryRemoved(const std::string& entry_id)
{
    Q_FOREACH(IndicatorEntryWidget* widget, m_entries)
    {
        if (widget->entry()->id() == entry_id) {
            widget->hide();
            m_layout->removeWidget(widget);
            m_indicatorsManager->removeIndicatorEntryWidget(widget);
            m_entries.removeOne(widget);
            break;
        }
    }
}

QList<IndicatorEntryWidget*> IndicatorsWidget::entries() const
{
    return m_entries;
}

#include "indicatorswidget.moc"
