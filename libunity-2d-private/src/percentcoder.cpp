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

#include <QUrl>

#include "percentcoder.h"

PercentCoder::PercentCoder(QObject* parent) :
    QObject(parent)
{
}

QString PercentCoder::encoded() const
{
    QString string(QUrl::toPercentEncoding(m_string));
    return string;
}

QString PercentCoder::unencoded() const
{
    return m_string;
}

void PercentCoder::setEncoded(const QString &string)
{
    QString str = QUrl::fromPercentEncoding(string.toAscii());
    if (str != m_string) {
        m_string = str;
        Q_EMIT stringChanged();
    }
}

void PercentCoder::setUnencoded(const QString& string)
{
    if (string != m_string) {
        m_string = string;
        Q_EMIT stringChanged();
    }
}

#include "percentcoder.moc"
