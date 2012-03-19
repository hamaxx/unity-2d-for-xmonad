/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Aurélien Gâteau <aurelien.gateau@canonical.com>
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

#ifndef BFB_H
#define BFB_H

#include "launcheritem.h"

// Qt
#include <QAbstractListModel>
#include <QMetaType>

class BfbItem : public LauncherItem
{
    Q_OBJECT
public:
    BfbItem();
    ~BfbItem();

    /* getters */
    virtual bool active() const;
    virtual int activeScreen() const;
    virtual bool running() const;
    virtual int windowCount() const;
    virtual bool urgent() const;
    virtual QString name() const;
    virtual QString icon() const;
    virtual bool launching() const;

    QObject* dashManager() const;
    void setDashManager(QObject* manager);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE virtual void createMenuActions();

private Q_SLOTS:
    void slotDashActiveChanged(bool active);
    void slotDashScreenChanged(int screen);

private:
    Q_DISABLE_COPY(BfbItem)
    bool m_active;
    int m_activeScreen;
    QObject* m_manager;
};

Q_DECLARE_METATYPE(BfbItem*)


class BfbModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* dashManager READ dashManager WRITE setDashManager)
public:
    BfbModel(QObject* parent = 0);
    ~BfbModel();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QObject* dashManager() const;
    void setDashManager(QObject* manager);

private:
    Q_DISABLE_COPY(BfbModel)
    BfbItem* m_bfbItem;
};

#endif // BFB_H
