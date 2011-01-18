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

#ifndef UNITY_PLACE_ENTRY_H
#define UNITY_PLACE_ENTRY_H

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QDBusMetaType>

#include "qdbusconnectionqml.h"


typedef QHash<QString, QString> QHashQStringQString;
Q_DECLARE_METATYPE(QHashQStringQString)

class UnityPlaceEntry : public QDBusConnectionQML
{
    Q_OBJECT



public:
    UnityPlaceEntry(QObject *parent = 0) : QDBusConnectionQML(parent)
    {
        qDBusRegisterMetaType<QHashQStringQString>();

        QObject::connect(this, SIGNAL(objectPathChanged(QString)), this, SLOT(connectSignals()));
        QObject::connect(this, SIGNAL(serviceChanged(QString)), this, SLOT(connectSignals()));
    }

    ~UnityPlaceEntry()
    {
    }

    Q_SLOT void connectSignals()
    {
        // FIXME: missing disconnection
        if(m_service.isEmpty() || m_objectPath.isEmpty())
            return;

        QObject::connect(m_interface, SIGNAL(EntryRendererInfoChanged()), this, SIGNAL(entryRendererInfoChanged()));
        QObject::connect(m_interface, SIGNAL(GlobalRendererInfoChanged()), this, SIGNAL(globalRendererInfoChanged()));
        QObject::connect(m_interface, SIGNAL(PlaceEntryInfoChanged()), this, SIGNAL(placeEntryInfoChanged()));
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
    Q_INVOKABLE inline void SetGlobalSearch(QString search, QHashQStringQString hints)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(search);
        argumentList << qVariantFromValue(hints);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "SetGlobalSearch", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }
    Q_INVOKABLE inline void SetSearch(QString search, QHashQStringQString hints)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(search);
        argumentList << qVariantFromValue(hints);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "SetSearch", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }
    Q_INVOKABLE inline void SetActive(bool is_active)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(is_active);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "SetActive", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }
    Q_INVOKABLE inline void SetActiveSection(uint section_id)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(section_id);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "SetActiveSection", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }

Q_SIGNALS:
    void entryRendererInfoChanged();
    void globalRendererInfoChanged();
    void placeEntryInfoChanged();
};
#endif
