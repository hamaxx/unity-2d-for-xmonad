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

#ifndef FDOTASK_H
#define FDOTASK_H

// Local

// Qt
#include <QObject>
#include <QWidget>

namespace SystemTray
{

class X11EmbedDelegate;

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject* parent)
    : QObject(parent)
    {}

    virtual void createWidget() = 0;
};

class FdoTask : public Task
{
Q_OBJECT
public:
    FdoTask(WId id, QObject *parent);
    ~FdoTask();

    virtual void createWidget();

Q_SIGNALS:
    void taskDeleted(WId);
    void widgetCreated(QWidget*);

private Q_SLOTS:
    void setupXEmbedDelegate();
    void slotClientEmbedded();

private:
    WId m_id;
    bool m_clientEmbedded;
    X11EmbedDelegate* m_widget;
};

} // namespace

#endif /* FDOTASK_H */
