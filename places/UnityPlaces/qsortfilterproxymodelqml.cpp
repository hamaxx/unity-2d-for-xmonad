#include "qsortfilterproxymodelqml.h"
#include <QDebug>

QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent) :
    QSortFilterProxyModel(parent), m_limit(-1)
{
}

void
QSortFilterProxyModelQML::updateRoleNames()
{
    setRoleNames(((QAbstractItemModel*)sourceModel())->roleNames());
}


QObject*
QSortFilterProxyModelQML::sourceModelQObject() const
{
    return (QAbstractItemModel*)sourceModel();
}

void
QSortFilterProxyModelQML::setSourceModelQObject(QObject *model)
{
    setSourceModel((QAbstractItemModel*)model);
    updateRoleNames();
    QObject::connect(model, SIGNAL(modelReset()),
                     this, SLOT(updateRoleNames()));
}

int
QSortFilterProxyModelQML::limit() const
{
    return m_limit;
}

void
QSortFilterProxyModelQML::setLimit(int limit)
{
    if(limit != m_limit)
    {
        m_limit = limit;
        emit limitChanged(limit);
        reset();
    }
}

int
QSortFilterProxyModelQML::rowCount(const QModelIndex & parent) const
{
    int actualRowCount = QSortFilterProxyModel::rowCount(parent);
    if(m_limit == -1)
        return actualRowCount;
    else
        return qMin(m_limit, actualRowCount);
}
