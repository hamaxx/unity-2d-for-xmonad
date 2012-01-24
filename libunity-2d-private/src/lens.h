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

// Local

// Qt
#include <QObject>
#include <QString>
#include <QMetaType>

// libunity-core
#include <UnityCore/Lens.h>

// dee-qt
#include "deelistmodel.h"

class Filters;

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
    Q_PROPERTY(DeeListModel* results READ results NOTIFY resultsChanged)
    Q_PROPERTY(DeeListModel* globalResults READ globalResults NOTIFY globalResultsChanged)
    Q_PROPERTY(DeeListModel* categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(Filters* filters READ filters NOTIFY filtersChanged)

    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(QString globalSearchQuery READ globalSearchQuery WRITE setGlobalSearchQuery NOTIFY globalSearchQueryChanged)

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
    DeeListModel* results() const;
    DeeListModel* globalResults() const;
    DeeListModel* categories() const;
    bool active() const;
    Filters* filters() const;
    QString searchQuery() const;
    QString globalSearchQuery() const;

    /* setters */
    void setActive(bool active);
    void setSearchQuery(const QString& search_query);
    void setGlobalSearchQuery(const QString& search_query);

    Q_INVOKABLE void activate(const QString& uri);
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
    void resultsChanged();
    void globalResultsChanged();
    void categoriesChanged();
    void activeChanged(bool);
    void filtersChanged();
    void searchFinished(std::string const&);
    void globalSearchFinished(std::string const&);
    void searchQueryChanged();
    void globalSearchQueryChanged();

private Q_SLOTS:
    void synchronizeStates();

private:
    void onResultsSwarmNameChanged(std::string);
    void onResultsChanged(unity::dash::Results::Ptr);
    void onGlobalResultsSwarmNameChanged(std::string);
    void onGlobalResultsChanged(unity::dash::Results::Ptr);
    void onCategoriesSwarmNameChanged(std::string);
    void onCategoriesChanged(unity::dash::Categories::Ptr);

    void onActivated(std::string const& uri, unity::dash::HandledType type, unity::dash::Lens::Hints const&);
    void fallbackActivate(const QString& uri);

    unity::dash::Lens::Ptr m_unityLens;
    DeeListModel* m_results;
    DeeListModel* m_globalResults;
    DeeListModel* m_categories;
    QString m_searchQuery;
    QString m_globalSearchQuery;
    Filters* m_filters;
};

Q_DECLARE_METATYPE(Lens*)

#endif // LENS_H
