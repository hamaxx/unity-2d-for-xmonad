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

#ifndef F10EVENTFILTER_H
#define F10EVENTFILTER_H

class QEvent;

class IndicatorEntryWidget;

// Qt
#include <QObject>
#include <QList>

class F10EventFilter : public QObject
{
Q_OBJECT
public:
    F10EventFilter(const QList<IndicatorEntryWidget*>*, bool);

    bool eventFilter(QObject*, QEvent*);

protected:
    virtual void beforeOpen(QObject*, QEvent*);

private:
    const QList<IndicatorEntryWidget*> *m_entries;
    bool m_onVisible;
};

#endif // F10EVENTFILTER_H
