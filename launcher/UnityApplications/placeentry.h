/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef PLACEENTRY_H
#define PLACEENTRY_H

#include "launcheritem.h"

// Hack until QtDee is in its own package
#include "../../places/QtDee/deelistmodel.h"

#include <QMetaType>
#include <QDBusInterface>
#include <QDBusArgument>

// https://wiki.ubuntu.com/Unity/Places#RendererInfo
struct RendererInfoStruct
{
    QString default_renderer;
    QString groups_model;
    QString results_model;
    QMap<QString, QString> renderer_hints;
};
Q_DECLARE_METATYPE(RendererInfoStruct)

// Marshalling and unmarshalling of RendererInfoStruct
QDBusArgument &operator<<(QDBusArgument &, const RendererInfoStruct &);
const QDBusArgument &operator>>(const QDBusArgument &, RendererInfoStruct &);


// https://wiki.ubuntu.com/Unity/Places#PlaceEntryInfo
struct PlaceEntryInfoStruct
{
    QString dbus_path;
    QString name;
    QString icon;
    uint position;
    QStringList mimetypes;
    bool sensitive;
    QString sections_model;
    QMap<QString, QString> hints;
    RendererInfoStruct entry_renderer_info;
    RendererInfoStruct global_renderer_info;
};
Q_DECLARE_METATYPE(PlaceEntryInfoStruct)
Q_DECLARE_METATYPE(QList<PlaceEntryInfoStruct>)

// Marshalling and unmarshalling of PlaceEntryInfoStruct
QDBusArgument &operator<<(QDBusArgument &, const PlaceEntryInfoStruct &);
const QDBusArgument &operator>>(const QDBusArgument &, PlaceEntryInfoStruct &);


class PlaceEntry : public LauncherItem
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString dbusName READ dbusName WRITE setDbusName)
    Q_PROPERTY(QString dbusObjectPath READ dbusObjectPath WRITE setDbusObjectPath)
    Q_PROPERTY(uint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QStringList mimetypes READ mimetypes WRITE setMimetypes)

public:
    PlaceEntry();
    PlaceEntry(const PlaceEntry& other);
    ~PlaceEntry();

    /* getters */
    virtual bool active() const;
    virtual bool running() const;
    virtual bool urgent() const;
    virtual QString name() const;
    virtual QString icon() const;
    virtual bool launching() const;
    QString fileName() const;
    QString groupName() const;
    QString dbusName() const;
    QString dbusObjectPath() const;
    uint position() const;
    QStringList mimetypes() const;

    /* setters */
    void setName(QString);
    void setIcon(QString);
    void setFileName(QString);
    void setGroupName(QString);
    void setDbusName(QString);
    void setDbusObjectPath(QString);
    void setPosition(uint);
    void setMimetypes(QStringList);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE virtual void createMenuActions();

    /* Connect to the remote representation of the entry on DBus and monitor
       changes. */
    void connectToRemotePlaceEntry();

    void updateInfo(const PlaceEntryInfoStruct& info);

Q_SIGNALS:
    void positionChanged(uint);

private:
    QString m_fileName;
    QString m_groupName;
    QString m_dbusName;
    QString m_dbusObjectPath;
    QString m_icon;
    QString m_name;
    uint m_position;
    QStringList m_mimetypes;
    DeeListModel* m_sections;
    QDBusInterface* m_dbusIface;

    void setSection(const QString&);
    void activateEntry(const int section);

private Q_SLOTS:
    void onRendererInfoChanged();
    void onPlaceEntryInfoChanged(const PlaceEntryInfoStruct&);

    /* Contextual menu callbacks */
    void onSectionTriggered();
};

Q_DECLARE_METATYPE(PlaceEntry*)

#endif // PLACEENTRY_H

