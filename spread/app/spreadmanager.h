/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#ifndef SPREADMANAGER_H
#define SPREADMANAGER_H

#include <QObject>

#include "spreadcontrol.h"

class SpreadView;

class SpreadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *currentSwitcher READ currentSwitcher NOTIFY currentSwitcherChanged)

public:
    explicit SpreadManager(QObject* parent = 0);
    ~SpreadManager();

    QObject *currentSwitcher() const;

Q_SIGNALS:
    void startCancelAndExit();
    void currentSwitcherChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void onScreenCountChanged(int);
    void onViewVisibleChanged(bool visible);

private:
    Q_DISABLE_COPY(SpreadManager);

    SpreadView *initSpread(int screen);

    QList<SpreadView *> m_viewList;
    SpreadControl m_control;
    SpreadView *m_grabber;
    SpreadView *m_focusedView;
};

#endif // SPREADMANAGER_H

