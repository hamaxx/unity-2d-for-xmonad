/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Lukasz 'sil2100' Zemczak <lukasz.zemczak@canonical.com>
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

#ifndef PERCENTCODER_H
#define PERCENTCODER_H

#include <QObject>

/* This element type can be used to covert URL Encoded strings to Unicode 
   QStrings and vice versa */
class PercentCoder : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString encoded READ encoded WRITE setEncoded NOTIFY stringChanged)
    Q_PROPERTY(QString unencoded READ unencoded WRITE setUnencoded NOTIFY stringChanged)

public:
    PercentCoder(QObject* parent = 0);

    QString encoded() const;
    QString unencoded() const;
    void setEncoded(const QString&);
    void setUnencoded(const QString&);

Q_SIGNALS:
    void stringChanged();

private:
    QString m_string;
};

#endif
