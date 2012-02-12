/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#ifndef IMAGEUTILITIES_H
#define IMAGEUTILITIES_H

#include <QObject>
#include <QColor>
#include <QUrl>
#include <QImage>

class ImageUtilities : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QColor averageColor READ averageColor NOTIFY averageColorChanged)

public:
    explicit ImageUtilities(QObject *parent = 0);

    // getters
    QUrl source() const;
    QColor averageColor() const;

    // setters
    void setSource(const QUrl&);

Q_SIGNALS:
    void sourceChanged();
    void averageColorChanged();

protected:
    QColor computeAverageColor(const QImage&);

private:
    QUrl m_source;
    QColor m_averageColor;
};

#endif // IMAGEUTILITIES_H
