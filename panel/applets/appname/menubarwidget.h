/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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

#ifndef MENUBARWIDGET_H
#define MENUBARWIDGET_H

// Qt
#include <QWidget>

// libunity-core
#include <UnityCore/UnityCore.h>

class QHBoxLayout;

class IndicatorEntryWidget;
class IndicatorsManager;

class MenuBarWidget : public QWidget, public sigc::trackable
{
Q_OBJECT
public:
    MenuBarWidget(IndicatorsManager*, QWidget* parent = 0);

    bool isEmpty() const;
    bool isOpened() const;

Q_SIGNALS:
    void menuBarClosed();
    void isEmptyChanged();

private Q_SLOTS:
    void updateIsEmpty();

private:
    Q_DISABLE_COPY(MenuBarWidget)

    IndicatorsManager* m_indicatorsManager;
    QHBoxLayout* m_layout;
    bool m_isEmpty;
    QList<IndicatorEntryWidget*> m_widgetList;

    void onObjectAdded(const unity::indicator::Indicator::Ptr&);
    void onEntryAdded(const unity::indicator::Entry::Ptr&);
};

#endif /* MENUBARWIDGET_H */
