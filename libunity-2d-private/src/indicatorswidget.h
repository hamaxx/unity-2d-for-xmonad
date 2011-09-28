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
#ifndef INDICATORSWIDGET_H
#define INDICATORSWIDGET_H

// Local

// libunity-core
#include <UnityCore/Indicator.h>
#include <UnityCore/IndicatorEntry.h>

// Qt
#include <QMap>
#include <QWidget>

// sigc++
#include <sigc++/connection.h>

class QHBoxLayout;

class IndicatorEntryWidget;
class IndicatorsManager;

class IndicatorsWidget : public QWidget, public sigc::trackable
{
Q_OBJECT
public:
    IndicatorsWidget(IndicatorsManager* manager);
    ~IndicatorsWidget();

    void addIndicator(const unity::indicator::Indicator::Ptr& indicator);
    void removeIndicator(const unity::indicator::Indicator::Ptr& indicator);

    QList<IndicatorEntryWidget*> entries() const;

private:
    QHBoxLayout* m_layout;
    IndicatorsManager* m_indicatorsManager;
    QMap<unity::indicator::Indicator::Ptr, QList<sigc::connection>> m_indicators_connections;
    QList<IndicatorEntryWidget*> m_entries;

    void onEntryAdded(const unity::indicator::Entry::Ptr& entry);
    void onEntryRemoved(const std::string& entry_id);
};

#endif /* INDICATORSWIDGET_H */
