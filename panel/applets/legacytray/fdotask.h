/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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
