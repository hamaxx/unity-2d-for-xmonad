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

#include "qdbusconnectionqml.h"

QDBusConnectionQML::QDBusConnectionQML(QObject *parent) : QObject(parent), m_interface(NULL)
{
    QObject::connect(this, SIGNAL(objectPathChanged(QString)), this, SLOT(connectToDBus()));
    QObject::connect(this, SIGNAL(serviceChanged(QString)), this, SLOT(connectToDBus()));
}

QDBusConnectionQML::~QDBusConnectionQML()
{
    if (m_interface != NULL)
        delete m_interface;
}

QString
QDBusConnectionQML::objectPath() const
{
    return m_objectPath;
}

QString
QDBusConnectionQML::service() const
{
    return m_service;
}

void
QDBusConnectionQML::setObjectPath(QString objectPath)
{
    m_objectPath = objectPath;
    emit objectPathChanged(m_objectPath);
}

void
QDBusConnectionQML::setService(QString service)
{
    m_service = service;
    emit serviceChanged(m_service);
}

void
QDBusConnectionQML::connectToDBus()
{
    if (m_interface != NULL)
        delete m_interface;

    if (m_service.isEmpty() || m_objectPath.isEmpty())
        return;

    m_interface = new QDBusInterface(m_service, m_objectPath);
}
