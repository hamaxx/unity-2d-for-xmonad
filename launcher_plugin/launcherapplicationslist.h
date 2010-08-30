#ifndef LAUNCHERAPPLICATIONSLIST_H
#define LAUNCHERAPPLICATIONSLIST_H

#include "launcherapplication.h"
#include <QAbstractListModel>
#include <QList>

class LauncherApplicationsList : public QAbstractListModel
{
    Q_OBJECT
public:
    LauncherApplicationsList(QObject *parent = 0);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
    void reload();
    QString desktopFilePathFromFavorite(QString favorite_id);
    QList<QLauncherApplication*> m_applications;
};

#endif // LAUNCHERAPPLICATIONSLIST_H
