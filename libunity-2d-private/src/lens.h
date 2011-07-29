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

#ifndef LENS_H
#define LENS_H

// Qt
#include <QObject>
#include <QString>
#include <QMetaType>

// libunity-core
#include <UnityCore/Lens.h>


class Lens : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString dbusName READ dbusName NOTIFY dbusNameChanged)
    Q_PROPERTY(QString dbusPath READ dbusPath NOTIFY dbusPathChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString searchHint READ searchHint NOTIFY searchHintChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(QString shortcut READ shortcut NOTIFY shortcutChanged)

public:
    explicit Lens(QObject *parent = 0);

    /* getters */
    QString id() const;
    QString dbusName() const;
    QString dbusPath() const;
    QString name() const;
    QString icon() const;
    QString description() const;
    QString searchHint() const;
    bool visible() const;
    QString shortcut() const;

    void setUnityLens(unity::dash::Lens::Ptr lens);

Q_SIGNALS:
    void idChanged(std::string);
    void dbusNameChanged(std::string);
    void dbusPathChanged(std::string);
    void nameChanged(std::string);
    void iconChanged(std::string);
    void descriptionChanged(std::string);
    void searchHintChanged(std::string);
    void visibleChanged(bool);
    void shortcutChanged(std::string);

private:
    unity::dash::Lens::Ptr m_unityLens;
};

Q_DECLARE_METATYPE(Lens*)

#endif // LENS_H
