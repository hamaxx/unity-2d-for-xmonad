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
#include <QString>

class QFileSystemWatcher;

/* Wrapper around GIO's g_app_info_get_default_for_type.
   To use it, set the contentType property and the desktopFile will contain the
   path to the desktop file of the application handling that kind of content.
*/
class GioDefaultApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString desktopFile READ desktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QString defaultDesktopFile READ defaultDesktopFile WRITE setDefaultDesktopFile NOTIFY defaultDesktopFileChanged)

public:
    GioDefaultApplication(QObject* parent=0);

    /* getters */
    QString desktopFile() const;
    QString contentType() const;
    QString defaultDesktopFile() const;

    /* setters */
    void setDefaultDesktopFile(const QString& defaultDesktopFile);
    void setContentType(const QString& contentType);

Q_SIGNALS:
    void desktopFileChanged();
    void contentTypeChanged();
    void defaultDesktopFileChanged();

private:
    Q_SLOT void updateDesktopFile();
    Q_SLOT void onMimeappsFileChanged();

    QString m_contentType;
    QString m_desktopFile;
    QString m_defaultDesktopFile;
    QFileSystemWatcher* m_mimeappsWatcher;
};

#endif // GIODEFAULTAPPLICATION

