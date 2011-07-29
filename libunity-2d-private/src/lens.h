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
    Q_PROPERTY(QString iconHint READ iconHint NOTIFY iconHintChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString searchHint READ searchHint NOTIFY searchHintChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(bool searchInGlobal READ searchInGlobal NOTIFY searchInGlobalChanged)
    Q_PROPERTY(QString shortcut READ shortcut NOTIFY shortcutChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
//    nux::RWProperty<Results::Ptr> results;
//    nux::RWProperty<Results::Ptr> global_results;
//    nux::RWProperty<Categories::Ptr> categories;
//    Q_PROPERTY(Results* results READ results NOTIFY resultsChanged)
//    Q_PROPERTY(Results* globalResults READ globalResults NOTIFY globalResultsChanged)
//    Q_PROPERTY(Categories* categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    explicit Lens(QObject *parent = 0);

    /* getters */
    QString id() const;
    QString dbusName() const;
    QString dbusPath() const;
    QString name() const;
    QString iconHint() const;
    QString description() const;
    QString searchHint() const;
    bool visible() const;
    bool searchInGlobal() const;
    QString shortcut() const;
    bool connected() const;
    bool active() const;

    void globalSearch(QString search_string);
    void search(QString search_string);
    void setUnityLens(unity::dash::Lens::Ptr lens);

Q_SIGNALS:
    void idChanged(std::string);
    void dbusNameChanged(std::string);
    void dbusPathChanged(std::string);
    void nameChanged(std::string);
    void iconHintChanged(std::string);
    void descriptionChanged(std::string);
    void searchHintChanged(std::string);
    void visibleChanged(bool);
    void searchInGlobalChanged(bool);
    void shortcutChanged(std::string);
    void connectedChanged(bool);
    void activeChanged(bool);
    void searchFinished(std::string const&);
    void globalSearchFinished(std::string const&);

private:
    unity::dash::Lens::Ptr m_unityLens;
};

Q_DECLARE_METATYPE(Lens*)

#endif // LENS_H
