#include "qsortfilterproxymodelqml.h"
#include <QDebug>

QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent) :
    QSortFilterProxyModel(parent), m_limit(-1)
{
    /* For some unknown reason the VisualDataModel wrapping our model does not tell
       the view to refresh itself when rows are inserted, removed or moved.

       Yes, a VisualDataModel is implicitly created when setting a QAbstractItemModel
       and the rowsInserted, rowsRemoved, rowsMoved signals are set to be
       transformed into the itemsInserted, itemsRemoved, itemsMoved signals that the
       view uses to refresh itself.
       Looks like the culprits might be:
            if (!parent.isValid())
       and similar in:
            QDeclarativeVisualDataModel::_q_rowsInserted
            QDeclarativeVisualDataModel::_q_rowsRemoved
            QDeclarativeVisualDataModel::_q_rowsMoved

       of file qdeclarativevisualitemmodel.cpp coupled with the fact that
       d->m_root is never initialized to the rootIndex we are actually using.
       Experimentations tended to show that it was not the only issue.

       In order to workaround that we call QAbstractItemModel::reset that VisualDataModel
       forwards inconditionally thus refreshing the view.

       Possibly related to bugs:
       http://bugreports.qt.nokia.com/browse/QTBUG-13038
       http://bugreports.qt.nokia.com/browse/QTBUG-13664

       When fixes for these bugs land in Ubuntu it would be worth removing the workaround
       and removing the rootIndex forcing as indicated.

       Unfortunately that produces a bug where the focus in a List/GridView is lost when
       the model is reset. See Applications.qml for a workaround.
    */
    QObject::connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
                     this, SLOT(reset()));
    QObject::connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                     this, SLOT(reset()));
    QObject::connect(this, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                     this, SLOT(reset()));
    QObject::connect(this, SIGNAL(layoutChanged()),
                     this, SLOT(reset()));
}

void
QSortFilterProxyModelQML::reset()
{
    QSortFilterProxyModel::reset();
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
