/*
 * This file is part of unity-qt
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
#include <QX11EmbedContainer>

namespace SystemTray
{

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject* parent)
    : QObject(parent)
    {}
    
    virtual QWidget* widget() = 0;
};

class FdoTask : public Task
{
Q_OBJECT
public:
    FdoTask(WId id, QObject *parent);
    ~FdoTask();
    
    virtual QWidget* widget();
    
Q_SIGNALS:
    void taskDeleted(WId);

protected:
    virtual bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void slotError(QX11EmbedContainer::Error);

private:
    WId m_id;
    QX11EmbedContainer* m_container;

    void doEmbedClient();
};

} // namespace

#endif /* FDOTASK_H */
