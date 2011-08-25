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

#ifndef LISTMODELWRAPPER_H
#define LISTMODELWRAPPER_H

// Qt
#include <QAbstractListModel>
#include <QList>

#include <sigc++/signal.h>
#include <vector>

template <class WrapperItem, class ItemClass>
class ListModelWrapper : public QAbstractListModel
{
    Q_ENUMS(Roles)

public:
    explicit ListModelWrapper(const std::vector<ItemClass>& list,
                              sigc::signal<void, ItemClass> itemAddedSignal,
                              sigc::signal<void, ItemClass> itemRemovedSignal,
                              QObject *parent = 0);
    ~ListModelWrapper();

    enum Roles {
        RoleItem
    };

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

private:
    void onItemAdded(ItemClass item);
    void onItemRemoved(ItemClass item);
    void onItemChanged(ItemClass item);

    QList<WrapperItem*> m_list;

    void addItem(ItemClass item, int index);
    void removeItem(ItemClass item, int index);
};


template <class WrapperItem, class ItemClass>
ListModelWrapper<WrapperItem, ItemClass>::ListModelWrapper(const std::vector<ItemClass>& list,
                                                           sigc::signal<void, ItemClass> itemAddedSignal,
                                                           sigc::signal<void, ItemClass> itemRemovedSignal,
                                                           QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[ListModelWrapper::RoleItem] = "item";
    setRoleNames(roles);

    for (unsigned int i=0; i<list.size(); i++) {
        addItem(list[i], i);
    }
    itemAddedSignal.connect(sigc::mem_fun(this, &ListModelWrapper::onItemAdded));
    itemRemovedSignal.connect(sigc::mem_fun(this, &ListModelWrapper::onItemRemoved));
}

template <class WrapperItem, class ItemClass>
ListModelWrapper<WrapperItem, ItemClass>::~ListModelWrapper()
{
    while (!m_list.isEmpty()) {
        delete m_list.takeFirst();
    }
}

template <class WrapperItem, class ItemClass>
int ListModelWrapper<WrapperItem, ItemClass>::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_list.count();
}

template <class WrapperItem, class ItemClass>
QVariant ListModelWrapper<WrapperItem, ItemClass>::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    WrapperItem* item = m_list.at(index.row());

    if (role == ListModelWrapper::RoleItem) {
        return QVariant::fromValue(item);
    } else {
        return QVariant();
    }
}

template <class WrapperItem, class ItemClass>
void ListModelWrapper<WrapperItem, ItemClass>::onItemAdded(ItemClass item)
{
    /* FIXME: figure out actual index of item; for now items are appended */
    int index = m_list.count();
    beginInsertRows(QModelIndex(), index, index);
    addItem(item, index);
    endInsertRows();
}

template <class WrapperItem, class ItemClass>
void ListModelWrapper<WrapperItem, ItemClass>::onItemRemoved(ItemClass item)
{
    // FIXME: lookup the actual index of item
    int index = m_list.count() - 1;
    beginInsertRows(QModelIndex(), index, index);
    removeItem(item, index);
    endInsertRows();
}

template <class WrapperItem, class ItemClass>
void ListModelWrapper<WrapperItem, ItemClass>::onItemChanged(ItemClass item)
{
    /* FIXME: broken */
//    QModelIndex itemIndex = index(m_list.indexOf(qobject_cast<Filter*>(sender())));
//    Q_EMIT dataChanged(itemIndex, itemIndex);
}

template <class WrapperItem, class ItemClass>
void ListModelWrapper<WrapperItem, ItemClass>::addItem(ItemClass item, int index)
{
    m_list.insert(index, new WrapperItem(item));
}

template <class WrapperItem, class ItemClass>
void ListModelWrapper<WrapperItem, ItemClass>::removeItem(ItemClass item, int index)
{
    delete m_list.takeAt(index);
}

#endif // LISTMODELWRAPPER_H
