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

#ifndef DEELISTMODEL_H
#define DEELISTMODEL_H

#include <QAbstractListModel>

#include "dee-shared-model-proxy.h"

Q_DECLARE_METATYPE(QList<qulonglong>)
Q_DECLARE_METATYPE(QList<QVariant>)
Q_DECLARE_METATYPE(QList<QList<QVariant>>)

class DeeListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    DeeListModel(QObject *parent = 0);
    ~DeeListModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* getters */
    QString objectPath() const;
    QString service() const;

    /* setters */
    void setObjectPath(QString objectPath);
    void setService(QString service);

signals:
    void objectPathChanged(QString);
    void serviceChanged(QString);

private:
    void connect();
    void load();

    ComCanonicalDeeModelInterface* m_dee_shared_model_proxy;

    QString m_columns;
    quint64 m_last_seqnum;
    QList<QList<QVariant>> m_rows;
    QList<qulonglong> m_seqnums;

    QString m_service;
    QString m_objectPath;
};


#endif // DEELISTMODEL_H
