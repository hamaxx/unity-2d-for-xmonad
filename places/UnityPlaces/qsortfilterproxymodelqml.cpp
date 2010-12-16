#include "qsortfilterproxymodelqml.h"
#include <QDebug>

QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent) :
    QSortFilterProxyModel(parent)
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
