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
#ifndef APPMENUINDICATORWIDGET_H
#define APPMENUINDICATORWIDGET_H

// Local
#include <indicatorwidget.h>

// Qt

/**
 * A specific version of IndicatorWidget, holding the appmenu
 */
class AppMenuIndicatorWidget : public IndicatorWidget
{
    Q_OBJECT
public:
    AppMenuIndicatorWidget(const unity::indicator::Indicator::Ptr& indicator);

protected:
    void addIndicatorEntryWidget(IndicatorEntryWidget*); // reimp
};

#endif /* APPMENUINDICATORWIDGET_H */
