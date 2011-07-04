/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#ifndef GIODEFAULTAPPLICATION_H
#define GIODEFAULTAPPLICATION_H

#include <QObject>

class GioDefaultApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString desktopFile READ desktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)

public:
    GioDefaultApplication(QObject* parent=0);

    /* getters */
    QString desktopFile() const;
    QString contentType() const;

    /* setters */
    void setContentType(const QString& contentType);

Q_SIGNALS:
    void desktopFileChanged();
    void contentTypeChanged();

private:
    void updateDesktopFile();

    QString m_contentType;
    QString m_desktopFile;
};

#endif // GIODEFAULTAPPLICATION

