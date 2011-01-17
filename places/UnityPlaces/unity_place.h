/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef UNITY_PLACE_H
#define UNITY_PLACE_H

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QDBusMetaType>

#include "qdbusconnectionqml.h"


class UnityPlace : public QDBusConnectionQML
{
    Q_OBJECT



public:
    UnityPlace(QObject *parent = 0) : QDBusConnectionQML(parent)
    {

        QObject::connect(this, SIGNAL(objectPathChanged(QString)), this, SLOT(connectSignals()));
        QObject::connect(this, SIGNAL(serviceChanged(QString)), this, SLOT(connectSignals()));
    }

    ~UnityPlace()
    {
    }

    Q_SLOT void connectSignals()
    {
        // FIXME: missing disconnection
        if(m_service.isEmpty() || m_objectPath.isEmpty())
            return;

        QObject::connect(m_interface, SIGNAL(EntryRemoved(QString)), this, SIGNAL(entryRemoved(QString)));
    }

    Q_INVOKABLE inline QString Introspect()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "Introspect", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QVariant Get(QString interface, QString propname)
    {
        if (m_interface == NULL)
            return QVariant();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(interface);
        argumentList << qVariantFromValue(propname);

        QDBusReply<QVariant> reply = m_interface->callWithArgumentList(QDBus::Block, "Get", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QVariant();
        }
      return reply.value();
    }
    Q_INVOKABLE inline void Set(QString interface, QString propname, QVariant value)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(interface);
        argumentList << qVariantFromValue(propname);
        argumentList << qVariantFromValue(value);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "Set", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }
    Q_INVOKABLE inline QHash<QString, QVariant> GetAll(QString interface)
    {
        if (m_interface == NULL)
            return QHash<QString, QVariant>();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(interface);

        QDBusReply<QHash<QString, QVariant>> reply = m_interface->callWithArgumentList(QDBus::Block, "GetAll", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QHash<QString, QVariant>();
        }
      return reply.value();
    }
    Q_INVOKABLE inline uint Activate(QString uri)
    {
        if (m_interface == NULL)
            return uint();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(uri);

        QDBusReply<uint> reply = m_interface->callWithArgumentList(QDBus::Block, "Activate", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return uint();
        }
      return reply.value();
    }

Q_SIGNALS:
    void entryRemoved(QString);
};
#endif
