/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

/* This code is largely inspired by Grégory Schlomoff’s qml-drag-drop project
   (https://bitbucket.org/gregschlom/qml-drag-drop/).
   FIXME: what is the license of the original source code? */

#ifndef DeclarativeMimeData_H
#define DeclarativeMimeData_H

#include <QtCore/QStringList>
#include <QtCore/QMimeData>

class DeclarativeMimeData : public QMimeData
{
    Q_OBJECT

    Q_PROPERTY(QStringList formats READ formats)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(QString html READ html)
    Q_PROPERTY(QStringList urls READ urls)

public:
    DeclarativeMimeData(const QMimeData* data);

    /* getters */
    QStringList urls() const;
};

Q_DECLARE_METATYPE(DeclarativeMimeData*)

#endif // DeclarativeMimeData

