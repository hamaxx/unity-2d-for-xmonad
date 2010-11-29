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

#include "placeentry.h"

#include <QDBusMetaType>
#include <QDebug>

// Marshall the RendererInfoStruct data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const RendererInfoStruct &r)
{
    argument.beginStructure();
    argument << r.default_renderer;
    argument << r.groups_model;
    argument << r.results_model;
    argument.beginMap(QVariant::String, QVariant::String);
    QMap<QString, QString>::const_iterator i;
    for (i = r.renderer_hints.constBegin(); i != r.renderer_hints.constEnd(); ++i) {
        argument.beginMapEntry();
        argument << i.key() << i.value();
        argument.endMapEntry();
    }
    argument.endMap();
    argument.endStructure();
    return argument;
}

// Retrieve the RendererInfoStruct data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, RendererInfoStruct &r)
{
    argument.beginStructure();
    argument >> r.default_renderer;
    argument >> r.groups_model;
    argument >> r.results_model;
    r.renderer_hints.clear();
    argument.beginMap();
    while (!argument.atEnd()) {
        QString key;
        QString value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        r.renderer_hints[key] = value;
    }
    argument.endMap();
    argument.endStructure();
    return argument;
}


// Marshall the PlaceEntryInfoStruct data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const PlaceEntryInfoStruct &p)
{
    argument.beginStructure();
    argument << p.dbus_path;
    argument << p.name;
    argument << p.icon;
    argument << p.position;
    argument.beginArray(QVariant::String);
    QStringList::const_iterator i;
    for (i = p.mimetypes.constBegin(); i != p.mimetypes.constEnd(); ++i) {
        argument << *i;
    }
    argument.endArray();
    argument << p.sensitive;
    argument << p.sections_model;
    argument.beginMap(QVariant::String, QVariant::String);
    QMap<QString, QString>::const_iterator j;
    for (j = p.hints.constBegin(); j != p.hints.constEnd(); ++j) {
        argument.beginMapEntry();
        argument << j.key() << j.value();
        argument.endMapEntry();
    }
    argument.endMap();
    argument << p.entry_renderer_info;
    argument << p.global_renderer_info;
    argument.endStructure();
    return argument;
}

// Retrieve the PlaceEntryInfoStruct data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, PlaceEntryInfoStruct &p)
{
    argument.beginStructure();
    argument >> p.dbus_path;
    argument >> p.name;
    argument >> p.icon;
    argument >> p.position;
    argument.beginArray();
    p.mimetypes.clear();
    while (!argument.atEnd()) {
        QString mimetype;
        argument >> mimetype;
        p.mimetypes.append(mimetype);
    }
    argument.endArray();
    argument >> p.sensitive;
    argument >> p.sections_model;
    p.hints.clear();
    argument.beginMap();
    while (!argument.atEnd()) {
        QString key;
        QString value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        p.hints[key] = value;
    }
    argument.endMap();
    /* The PlaceEntryInfoChanged signal on the com.canonical.Unity.PlaceEntry
       interface omits the two RenderingInfo structs. */
    if (!argument.atEnd()) {
        argument >> p.entry_renderer_info;
        argument >> p.global_renderer_info;
    }
    argument.endStructure();
    return argument;
}


static const char* UNITY_PLACE_ENTRY_INTERFACE = "com.canonical.Unity.PlaceEntry";
static const char* SECTION_PROPERTY = "section";

PlaceEntry::PlaceEntry() :
    m_position(0),
    m_sensitive(true),
    m_sections(NULL),
    m_online(false),
    m_dbusIface(NULL)
{
    // FIXME: this is not the right place to do this…
    qDBusRegisterMetaType<RendererInfoStruct>();
    qDBusRegisterMetaType<PlaceEntryInfoStruct>();
    qDBusRegisterMetaType<QList<PlaceEntryInfoStruct> >();
}

PlaceEntry::PlaceEntry(const PlaceEntry& other) :
    m_fileName(other.m_fileName),
    m_groupName(other.m_groupName),
    m_dbusName(other.m_dbusName),
    m_dbusObjectPath(other.m_dbusObjectPath),
    m_icon(other.m_icon),
    m_name(other.m_name),
    m_position(other.m_position),
    m_mimetypes(other.m_mimetypes),
    m_sensitive(other.m_sensitive),
    m_entryRendererName(other.m_entryRendererName)
{
    setSections(other.m_sections);
    setHints(other.m_hints);
}

PlaceEntry::~PlaceEntry()
{
    delete m_sections;
    delete m_dbusIface;
}

bool
PlaceEntry::active() const
{
    // TODO: implement me
    return false;
}

bool
PlaceEntry::running() const
{
    return false;
}

bool
PlaceEntry::urgent() const
{
    return false;
}

QString
PlaceEntry::name() const
{
    return m_name;
}

void
PlaceEntry::setName(QString name)
{
    m_name = name;
}

QString
PlaceEntry::icon() const
{
    return m_icon;
}

void
PlaceEntry::setIcon(QString icon)
{
    m_icon = icon;
}

bool
PlaceEntry::launching() const
{
    /* This basically means no launching animation when opening the device.
       Unity behaves likes this. */
    return false;
}

QString
PlaceEntry::fileName() const
{
    return m_fileName;
}

void
PlaceEntry::setFileName(QString fileName)
{
    m_fileName = fileName;
}

QString
PlaceEntry::groupName() const
{
    return m_groupName;
}

void
PlaceEntry::setGroupName(QString groupName)
{
    m_groupName = groupName;
}

QString
PlaceEntry::dbusName() const
{
    return m_dbusName;
}

QString
PlaceEntry::dbusObjectPath() const
{
    return m_dbusObjectPath;
}

uint
PlaceEntry::position() const
{
    return m_position;
}

QStringList
PlaceEntry::mimetypes() const
{
    return m_mimetypes;
}

bool
PlaceEntry::sensitive() const
{
    return m_sensitive;
}

DeeListModel*
PlaceEntry::sections() const
{
    return m_sections;
}

QMap<QString, QVariant>
PlaceEntry::hints() const
{
    return m_hints;
}

QString
PlaceEntry::entryRendererName() const
{
    return m_entryRendererName;
}

bool
PlaceEntry::online() const
{
    return m_online;
}

void
PlaceEntry::setDbusName(QString dbusName)
{
    m_dbusName = dbusName;
}

void
PlaceEntry::setDbusObjectPath(QString dbusObjectPath)
{
    m_dbusObjectPath = dbusObjectPath;
}

void
PlaceEntry::setPosition(uint position)
{
    if (position != m_position) {
        m_position = position;
        emit positionChanged(position);
    }
}

void
PlaceEntry::setMimetypes(QStringList mimetypes)
{
    m_mimetypes = mimetypes;
    emit mimetypesChanged();
}

void
PlaceEntry::setSensitive(bool sensitive)
{
    if (sensitive != m_sensitive) {
        m_sensitive = sensitive;
        emit sensitiveChanged(sensitive);
    }
}

void
PlaceEntry::setSections(DeeListModel* sections)
{
    if (sections == NULL) {
        return;
    }
    if (m_sections != NULL) {
        delete m_sections;
    }
    m_sections = sections;
    emit sectionsChanged();
}

void
PlaceEntry::setHints(QMap<QString, QVariant> hints)
{
    m_hints = hints;
    emit hintsChanged();
}

void
PlaceEntry::setEntryRendererName(QString entryRendererName)
{
    if (entryRendererName != m_entryRendererName) {
        m_entryRendererName = entryRendererName;
        emit entryRendererNameChanged();
    }
}

void
PlaceEntry::activate()
{
    activateEntry(0);
}

void
PlaceEntry::activateEntry(const int section)
{
    if (!m_sensitive) {
        return;
    }

    QDBusInterface iface("com.canonical.UnityQt", "/dash", "local.DashDeclarativeView");
    iface.call("activatePlaceEntry", m_fileName, m_groupName, section);
}

void
PlaceEntry::createMenuActions()
{
    if (!m_sensitive) {
        return;
    }

    for(int i = 0; i < m_sections->rowCount(); ++i) {
        QAction* section = new QAction(m_menu);
        section->setText(m_sections->data(m_sections->index(i)).toString());
        section->setProperty(SECTION_PROPERTY, QVariant(i));
        m_menu->addAction(section);
        QObject::connect(section, SIGNAL(triggered()), this, SLOT(onSectionTriggered()));
    }
}

void
PlaceEntry::onSectionTriggered()
{
    QAction* action = static_cast<QAction*>(sender());
    int section = action->property(SECTION_PROPERTY).toInt();
    hideMenu(true);
    activateEntry(section);
}

void
PlaceEntry::connectToRemotePlaceEntry()
{
    if (m_online) return;

    m_dbusIface = new QDBusInterface(m_dbusName, m_dbusObjectPath,
                                     UNITY_PLACE_ENTRY_INTERFACE);
    if (!m_dbusIface->isValid()) {
        m_online = false;
        return;
    }

    // Connect to RendererInfoChanged and PlaceEntryInfoChanged signals
    QDBusConnection connection = m_dbusIface->connection();
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_ENTRY_INTERFACE,
                       "RendererInfoChanged", this,
                       SLOT(onRendererInfoChanged(const RendererInfoStruct&)));
    connection.connect(m_dbusName, m_dbusObjectPath, UNITY_PLACE_ENTRY_INTERFACE,
                       "PlaceEntryInfoChanged", this,
                       SLOT(onPlaceEntryInfoChanged(const PlaceEntryInfoStruct&)));

    m_online = true;
}

void
PlaceEntry::updateInfo(const PlaceEntryInfoStruct& info)
{
    if (info.name != "") {
        setName(info.name);
    }
    setIcon(info.icon);
    setPosition(info.position);
    setMimetypes(info.mimetypes);
    setSensitive(info.sensitive);
    setSection(info.sections_model);

    QMap<QString, QVariant> hints;
    QMap<QString, QString>::const_iterator i;
    for(i = info.hints.constBegin(); i != info.hints.constEnd(); ++i) {
        hints[i.key()] = QVariant(i.value());
    }
    setHints(hints);

    setEntryRendererName(info.entry_renderer_info.default_renderer);
    /*const QString& eGroupsModelName = info.entry_renderer_info.groups_model;
    if (entryGroupsModelName() != eGroupsModelName) {
        setEntryGroupsModelName(eGroupsModelName);
        setEntryGroupsModel(NULL);
    }*/
    /*const QString& eResultsModelName = info.entry_renderer_info.results_model;
    if (entryResultsModelName() != eResultsModelName) {
        setEntryResultsModelName(eResultsModelName);
        setEntryResultsModel(NULL);
    }*/
    //setEntryRendererHints(info.entry_renderer_info.renderer_hints);

    //setGlobalRendererName(info.global_renderer_info.default_renderer);
    /*const QString& gGroupsModelName = info.global_renderer_info.groups_model;
    if (globalGroupsModelName() != gGroupsModelName) {
        setGlobalGroupsModelName(gGroupsModelName);
        setGlobalGroupsModel(NULL);
    }*/
    /*const QString& gResultsModelName = info.global_renderer_info.results_model;
    if (globalResultsModelName() != gResultsModelName) {
        setGlobalResultsModelName(gResultsModelName);
        setGlobalResultsModel(NULL);
    }*/
    //setGlobalRendererHints(info.global_renderer_info.renderer_hints);

    //emit updated();
    //emit rendererInfoChanged();
}

void
PlaceEntry::setSection(const QString& sectionModelName)
{
    DeeListModel* sections = new DeeListModel;
    QString path = m_dbusName;
    path.replace(".", "/");
    sections->setObjectPath("/com/canonical/dee/model/" + path + "/SectionsModel");
    sections->setService(sectionModelName);
    setSections(sections);
}

void
PlaceEntry::onRendererInfoChanged(const RendererInfoStruct& r)
{
    // TODO
    //emit rendererInfoChanged();
}

void
PlaceEntry::onPlaceEntryInfoChanged(const PlaceEntryInfoStruct& p)
{
    updateInfo(p);
    //emit rendererInfoChanged();
}

