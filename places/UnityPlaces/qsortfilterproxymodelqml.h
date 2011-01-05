#ifndef QSORTFILTERPROXYMODELQML_H
#define QSORTFILTERPROXYMODELQML_H

#include <QSortFilterProxyModel>

class QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* model READ sourceModelQObject WRITE setSourceModelQObject)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    /* getters */
    QObject* sourceModelQObject() const;

    /* setters */
    void setSourceModelQObject(QObject *model);

private slots:
    void updateRoleNames();
};

#endif // QSORTFILTERPROXYMODELQML_H
