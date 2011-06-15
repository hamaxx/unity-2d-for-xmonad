/*
 * Unity2d
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

// Self
#include "unity2dapplication.h"

// Qt

AbstractX11EventFilter::~AbstractX11EventFilter()
{
    Unity2dApplication* application = Unity2dApplication::instance();
    if (application != NULL) {
        application->removeX11EventFilter(this);
    }
}

Unity2dApplication::Unity2dApplication(int& argc, char** argv)
: QApplication(argc, argv)
{
}

Unity2dApplication::~Unity2dApplication()
{
    qDeleteAll(m_x11EventFilters);
}

Unity2dApplication* Unity2dApplication::instance()
{
    return qobject_cast<Unity2dApplication*>(QCoreApplication::instance());
}

void Unity2dApplication::installX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.append(filter);
}

void Unity2dApplication::removeX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.removeAll(filter);
}

bool Unity2dApplication::x11EventFilter(XEvent* event)
{
    Q_FOREACH(AbstractX11EventFilter* filter, m_x11EventFilters) {
        if (filter->x11EventFilter(event)) {
            return true;
        }
    }
    return QApplication::x11EventFilter(event);
}

#include <unity2dapplication.moc>
