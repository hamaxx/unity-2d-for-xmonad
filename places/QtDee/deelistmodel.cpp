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
    qDBusRegisterMetaType<QList<uint>>();
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
        /* FIXME: these simplistic connections fail at runtime with the following
                  error messages:

           Object::connect: No such signal local::Merged::RowsAdded(QList<QList<QVariant>>, QList<uint>, QList<qulonglong>)
           Object::connect: No such signal local::Merged::RowsRemoved(QList<uint>,QList<qulonglong>)

           As it turns out the DBusInterface object (m_interface) does not have
           these 2 signals as proven by the following code:

           #include <QMetaMethod>
           const QMetaObject* meta = m_dee_shared_model_proxy->metaObject();
           for (int i = 0; i < meta->methodCount(); ++i)
              qDebug() << i << " " << meta->method(i).methodType() << ": " << meta->method(i).signature() << " -> " << meta->method(i).typeName();

           It could be an instance of that Qt bug that was filed and closed:

           http://bugreports.qt.nokia.com/browse/QTBUG-5563
        */
        //QObject::connect(m_dee_shared_model_proxy, SIGNAL(RowsAdded(QList<QList<QVariant>>, QList<uint>, QList<qulonglong>)), this, SLOT(load()));
        //QObject::connect(m_dee_shared_model_proxy, SIGNAL(RowsRemoved(QList<uint>,QList<qulonglong>)), this, SLOT(load()));

        /* FIXME: that way of connecting works however connecting to 'load' for
                  every single change is wrong and very heavy */
        QDBusConnection::sessionBus().connect(m_service, m_objectPath, QString(), QString("RowsAdded"), this, SLOT(load()));
        QDBusConnection::sessionBus().connect(m_service, m_objectPath, QString(), QString("RowsRemoved"), this, SLOT(load()));
        QDBusConnection::sessionBus().connect(m_service, m_objectPath, QString(), QString("RowsChanged"), this, SLOT(load()));
        load();
    }
}

void
DeeListModel::load()
{
    QDBusPendingReply<QString, qulonglong, QList<QList<QVariant>>, QList<qulonglong>> reply = m_dee_shared_model_proxy->call("Clone");
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

    QHash<int, QByteArray> roles;
    QString column;

    for (int index=0; index<m_columns.length(); index++)
    {
        column = QString("column_%1").arg(index);
        roles[index] = column.toAscii();
    }

    setRoleNames(roles);
    reset();
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
