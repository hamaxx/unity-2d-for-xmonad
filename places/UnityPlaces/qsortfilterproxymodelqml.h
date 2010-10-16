#ifndef QSORTFILTERPROXYMODELQML_H
#define QSORTFILTERPROXYMODELQML_H

#include <QSortFilterProxyModel>

class QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* model READ sourceModelQObject WRITE setSourceModelQObject)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    QObject* sourceModelQObject() const;
    void setSourceModelQObject(QObject *model);
signals:

public slots:

private slots:
    void reset();
    void updateRoleNames();
};

#endif // QSORTFILTERPROXYMODELQML_H
