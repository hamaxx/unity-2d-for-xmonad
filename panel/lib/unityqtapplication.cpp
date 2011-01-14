/*
 * Unity2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "unityqtapplication.h"

// Qt

AbstractX11EventFilter::~AbstractX11EventFilter()
{
    Unity2dApplication::instance()->removeX11EventFilter(this);
}

Unity2dApplication::UnityQtApplication(int& argc, char** argv)
: QApplication(argc, argv)
{
}

Unity2dApplication* UnityQtApplication::instance()
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

#include <unityqtapplication.moc>
