/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#ifndef ICONUTILITIES_H
#define ICONUTILITIES_H

#include <QObject>
#include <QUrl>
#include <QSize>
#include <QVariant>
#include <QDeclarativeEngine>

class IconUtilities : public QObject
{
    Q_OBJECT

public :
    explicit IconUtilities(QDeclarativeEngine *engine);

    Q_INVOKABLE QList<QVariant> getColorsFromIcon(QUrl source, QSize size) const;

private:
    QDeclarativeEngine* m_engine;
};

#endif // ICONUTILITIES_H
