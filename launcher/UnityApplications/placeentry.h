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

#include "deelistmodel.h"

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
    Q_PROPERTY(QStringList mimetypes READ mimetypes WRITE setMimetypes NOTIFY mimetypesChanged)
    Q_PROPERTY(bool sensitive READ sensitive WRITE setSensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(DeeListModel* sections READ sections WRITE setSections NOTIFY sectionsChanged)
    Q_PROPERTY(QMap hints READ hints WRITE setHints NOTIFY hintsChanged)

    /* Entry rendering info */
    Q_PROPERTY(QString entryRendererName READ entryRendererName WRITE setEntryRendererName NOTIFY entryRendererNameChanged)
    Q_PROPERTY(QString entryGroupsModelName READ entryGroupsModelName WRITE setEntryGroupsModelName NOTIFY entryGroupsModelNameChanged)
    Q_PROPERTY(DeeListModel* entryGroupsModel READ entryGroupsModel WRITE setEntryGroupsModel NOTIFY entryGroupsModelChanged)
    Q_PROPERTY(QString entryResultsModelName READ entryResultsModelName WRITE setEntryResultsModelName NOTIFY entryResultsModelNameChanged)
    Q_PROPERTY(DeeListModel* entryResultsModel READ entryResultsModel WRITE setEntryResultsModel NOTIFY entryResultsModelChanged)
    Q_PROPERTY(QMap entryRendererHints READ entryRendererHints WRITE setEntryRendererHints NOTIFY entryRendererHintsChanged)

    /* Global rendering info */
    Q_PROPERTY(QString globalRendererName READ globalRendererName WRITE setGlobalRendererName NOTIFY globalRendererNameChanged)
    Q_PROPERTY(QString globalGroupsModelName READ globalGroupsModelName WRITE setGlobalGroupsModelName NOTIFY globalGroupsModelNameChanged)
    Q_PROPERTY(DeeListModel* globalGroupsModel READ globalGroupsModel WRITE setGlobalGroupsModel NOTIFY globalGroupsModelChanged)
    Q_PROPERTY(QString globalResultsModelName READ globalResultsModelName WRITE setGlobalResultsModelName NOTIFY globalResultsModelNameChanged)
    Q_PROPERTY(DeeListModel* globalResultsModel READ globalResultsModel WRITE setGlobalResultsModel NOTIFY globalResultsModelChanged)
    Q_PROPERTY(QMap globalRendererHints READ globalRendererHints WRITE setGlobalRendererHints NOTIFY globalRendererHintsChanged)

public:
    PlaceEntry(QObject* parent = 0);
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
    bool sensitive() const;
    DeeListModel* sections() const;
    QMap<QString, QVariant> hints() const;

    QString entryRendererName() const;
    QString entryGroupsModelName() const;
    DeeListModel* entryGroupsModel();
    QString entryResultsModelName() const;
    DeeListModel* entryResultsModel();
    QMap<QString, QVariant> entryRendererHints() const;

    QString globalRendererName() const;
    QString globalGroupsModelName() const;
    DeeListModel* globalGroupsModel();
    QString globalResultsModelName() const;
    DeeListModel* globalResultsModel();
    QMap<QString, QVariant> globalRendererHints() const;

    /* setters */
    void setName(QString);
    void setIcon(QString);
    void setFileName(QString);
    void setGroupName(QString);
    void setDbusName(QString);
    void setDbusObjectPath(QString);
    void setPosition(uint);
    void setMimetypes(QStringList);
    void setSensitive(bool);
    void setSections(DeeListModel*);
    void setHints(QMap<QString, QVariant>);

    void setEntryRendererName(QString);
    void setEntryGroupsModelName(QString);
    void setEntryGroupsModel(DeeListModel*);
    void setEntryResultsModelName(QString);
    void setEntryResultsModel(DeeListModel*);
    void setEntryRendererHints(QMap<QString, QVariant>);

    void setGlobalRendererName(QString);
    void setGlobalGroupsModelName(QString);
    void setGlobalGroupsModel(DeeListModel*);
    void setGlobalResultsModelName(QString);
    void setGlobalResultsModel(DeeListModel*);
    void setGlobalRendererHints(QMap<QString, QVariant>);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE virtual void createMenuActions();

    /* Connect to the remote representation of the entry on DBus and monitor
       changes. */
    void connectToRemotePlaceEntry();

    void updateInfo(const PlaceEntryInfoStruct& info);

Q_SIGNALS:
    void positionChanged(uint);
    void mimetypesChanged();
    void sensitiveChanged(bool);
    void sectionsChanged();
    void hintsChanged();

    void entryRendererNameChanged();
    void entryGroupsModelNameChanged();
    void entryGroupsModelChanged();
    void entryResultsModelNameChanged();
    void entryResultsModelChanged();
    void entryRendererHintsChanged();

    void globalRendererNameChanged();
    void globalGroupsModelNameChanged();
    void globalGroupsModelChanged();
    void globalResultsModelNameChanged();
    void globalResultsModelChanged();
    void globalRendererHintsChanged();

    void updated();
    void rendererInfoChanged();

private:
    QString m_fileName;
    QString m_groupName;
    QString m_dbusName;
    QString m_dbusObjectPath;
    QString m_icon;
    QString m_name;
    uint m_position;
    QStringList m_mimetypes;
    bool m_sensitive;
    DeeListModel* m_sections;
    /* m_hints should be a QMap<QString, QString> really, but it has to hold
       QVariant values in order to allow exposing it as a property (see
       http://doc.trolltech.com/properties.html#requirements-for-declaring-properties). */
    QMap<QString, QVariant> m_hints;

    QString m_entryRendererName;
    QString m_entryGroupsModelName;
    DeeListModel* m_entryGroupsModel;
    QString m_entryResultsModelName;
    DeeListModel* m_entryResultsModel;
    /* The remark about m_hints also applies to m_entryRendererHints. */
    QMap<QString, QVariant> m_entryRendererHints;

    QString m_globalRendererName;
    QString m_globalGroupsModelName;
    DeeListModel* m_globalGroupsModel;
    QString m_globalResultsModelName;
    DeeListModel* m_globalResultsModel;
    /* The remark about m_hints also applies to m_globalRendererHints. */
    QMap<QString, QVariant> m_globalRendererHints;

    QDBusInterface* m_dbusIface;

    void setSection(const QString&);
    void activateEntry(const int section);

    void startRemotePlaceOnDemand();

private Q_SLOTS:
    void onRendererInfoChanged(const RendererInfoStruct&);

    /* Contextual menu callbacks */
    void onSectionTriggered();
};

Q_DECLARE_METATYPE(PlaceEntry*)

#endif // PLACEENTRY_H

