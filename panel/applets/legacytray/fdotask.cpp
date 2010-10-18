/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "fdotask.h"

// Local

// uqpanel
#include <debug_p.h>

// Qt
#include <QEvent>
#include <QX11EmbedContainer>

namespace SystemTray
{

FdoTask::FdoTask(WId id, QObject* parent)
: Task(parent)
, m_id(id)
, m_container(new QX11EmbedContainer)
{
    m_container->setFixedSize(22, 22);
    m_container->installEventFilter(this);
    connect(m_container, SIGNAL(clientClosed()), SLOT(deleteLater()));
    connect(m_container, SIGNAL(error(QX11EmbedContainer::Error)), SLOT(slotError(QX11EmbedContainer::Error)));
}

FdoTask::~FdoTask()
{
    UQ_VAR(m_id);
    taskDeleted(m_id);
    m_container->deleteLater();
}

QWidget* FdoTask::widget()
{
    return m_container;
}

void FdoTask::slotError(QX11EmbedContainer::Error error)
{
    UQ_VAR(m_id);
    UQ_VAR(error);
}

void FdoTask::doEmbedClient()
{
    m_container->embedClient(m_id);
}

bool FdoTask::eventFilter(QObject* , QEvent* event)
{
    if (event->type() == QEvent::Show) {
        doEmbedClient();
        m_container->removeEventFilter(this);
    }
    return false;
}


} // namespace

#include "fdotask.moc"
