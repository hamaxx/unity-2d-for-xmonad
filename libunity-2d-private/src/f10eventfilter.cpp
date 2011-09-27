/*
 * Copyright (C) 2011 Canonical Ltd.
 *
 * Authors:
 * - Łukasz 'sil2100' Zemczak <lukasz.zemczak@canonical.com>
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
#include "f10eventfilter.h"

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Unity
#include <indicatorentrywidget.h>
#include <unity2dpanel.h>

// Qt
#include <QList>
#include <QEvent>

F10EventFilter::F10EventFilter(const QList<IndicatorEntryWidget*>* entries, bool onVisible)
: m_entries(entries)
, m_onVisible(onVisible)
{
}

bool F10EventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == Unity2dPanel::SHOW_FIRST_MENU_EVENT) {
        BamfApplication* app = BamfMatcher::get_default().active_application();
        bool isUserVisibleApp = app ? app->user_visible() : false;
        if ((isUserVisibleApp && m_onVisible) || (!isUserVisibleApp && !m_onVisible)) {
            beforeOpen(watched, event);

            if (m_entries != NULL && !m_entries->isEmpty()) {
                IndicatorEntryWidget* el = m_entries->first();
                if (el != NULL) {
                    el->showMenu(Qt::NoButton);
                }
            }
            return true;
        } else {
            return false;
        }
    } else {
        return QObject::eventFilter(watched, event);
    }
}

void F10EventFilter::beforeOpen(QObject* watched, QEvent* event)
{
}

#include "f10eventfilter.moc"
