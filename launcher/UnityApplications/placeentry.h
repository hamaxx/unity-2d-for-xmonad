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

#include <QMetaType>

class PlaceEntry : public LauncherItem
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString dbusObjectPath READ dbusObjectPath WRITE setDbusObjectPath)

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
    QString dbusObjectPath() const;

    /* setters */
    void setName(QString);
    void setIcon(QString);
    void setFileName(QString);
    void setGroupName(QString);
    void setDbusObjectPath(QString);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE virtual void createMenuActions();

private:
    QString m_fileName;
    QString m_groupName;
    QString m_dbusObjectPath;
    QString m_icon;
    QString m_name;
};

Q_DECLARE_METATYPE(PlaceEntry*)

#endif // PLACEENTRY_H

