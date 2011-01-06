/*
 * UnityQt
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
    UnityQtApplication::instance()->removeX11EventFilter(this);
}

UnityQtApplication::UnityQtApplication(int& argc, char** argv)
: QApplication(argc, argv)
{
}

UnityQtApplication* UnityQtApplication::instance()
{
    return static_cast<UnityQtApplication*>(QCoreApplication::instance());
}

void UnityQtApplication::installX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.append(filter);
}

void UnityQtApplication::removeX11EventFilter(AbstractX11EventFilter* filter)
{
    m_x11EventFilters.removeAll(filter);
}

bool UnityQtApplication::x11EventFilter(XEvent* event)
{
    Q_FOREACH(AbstractX11EventFilter* filter, m_x11EventFilters) {
        if (filter->x11EventFilter(event)) {
            return true;
        }
    }
    return QApplication::x11EventFilter(event);
}
