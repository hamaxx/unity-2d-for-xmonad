/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "deelistmodel.h"

#include <QHash>
#include <QByteArray>
#include <QDBusMetaType>
#include <QDBusPendingReply>

#include <QDebug>


DeeListModel::DeeListModel(QObject *parent) :
    QAbstractListModel(parent), m_dee_shared_model_proxy(NULL)
{
    qDBusRegisterMetaType<QList<qulonglong>>();
    qDBusRegisterMetaType<QList<QVariant>>();
    qDBusRegisterMetaType<QList<QList<QVariant>>>();
}

DeeListModel::~DeeListModel()
{
    if (m_dee_shared_model_proxy != NULL)
        delete m_dee_shared_model_proxy;
}

QString
DeeListModel::objectPath() const
{
    return m_objectPath;
}

QString
DeeListModel::service() const
{
    return m_service;
}

void
DeeListModel::setObjectPath(QString objectPath)
{
    m_objectPath = objectPath;
    connect();
}

void
DeeListModel::setService(QString service)
{
    m_service = service;
    connect();
}


void
DeeListModel::connect()
{
    if (m_dee_shared_model_proxy != NULL)
    {
        delete m_dee_shared_model_proxy;
        m_dee_shared_model_proxy = NULL;
    }

    if (!m_service.isEmpty() && !m_objectPath.isEmpty())
    {
        m_dee_shared_model_proxy = new QDBusInterface(m_service, m_objectPath);
        load();
    }
}

void
DeeListModel::load()
{
    QDBusPendingReply<QString, quint64, QList<QList<QVariant>>, QList<qulonglong>> reply = m_dee_shared_model_proxy->call("Clone");
    reply.waitForFinished();
    if (reply.isError())
    {
        qWarning() << reply.error();
        return;
    }

    m_columns = reply.argumentAt<0>();
    m_last_seqnum = reply.argumentAt<1>();
    m_rows = reply.argumentAt<2>();
    m_seqnums = reply.argumentAt<3>();

    qDebug() << m_columns << m_last_seqnum /*<< m_rows << m_seqnums*/;

    QHash<int, QByteArray> roles;
    QString column;

    for (int index=0; index<m_columns.length(); index++)
    {
        column = QString("column_%1").arg(index);
        roles[index] = column.toAscii();
    }

    setRoleNames(roles);
}


int
DeeListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_rows.size();
}

QVariant
DeeListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    return m_rows[index.row()][role];
}
