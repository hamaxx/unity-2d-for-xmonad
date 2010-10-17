#ifndef QSORTFILTERPROXYMODELQML_H
#define QSORTFILTERPROXYMODELQML_H

#include <QSortFilterProxyModel>

class QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* model READ sourceModelQObject WRITE setSourceModelQObject)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* getters */
    QObject* sourceModelQObject() const;
    int limit() const;

    /* setters */
    void setSourceModelQObject(QObject *model);
    void setLimit(int limit);

signals:
    void limitChanged(int limit);

private slots:
    void reset();
    void updateRoleNames();

private:
    int m_limit;
};

#endif // QSORTFILTERPROXYMODELQML_H
