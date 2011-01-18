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

#ifndef QDBUSCONNECTIONQML_H
#define QDBUSCONNECTIONQML_H

#include <QObject>
#include <QDBusInterface>

class QDBusConnectionQML : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    QDBusConnectionQML(QObject *parent = 0);
    ~QDBusConnectionQML();

    /* getters */
    QString objectPath() const;
    QString service() const;

    /* setters */
    void setObjectPath(QString objectPath);
    void setService(QString service);

private slots:
    void connectToDBus();

signals:
    void objectPathChanged(QString);
    void serviceChanged(QString);

protected:
    QString m_service;
    QString m_objectPath;
    QDBusInterface* m_interface;
};

#endif

