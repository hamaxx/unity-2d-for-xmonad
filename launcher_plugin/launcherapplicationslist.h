#ifndef LAUNCHERAPPLICATIONSLIST_H
#define LAUNCHERAPPLICATIONSLIST_H

#include "launcherapplication.h"
#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QString>
#include <QObject>

class LauncherApplicationsList : public QAbstractListModel
{
    Q_OBJECT
public:
    LauncherApplicationsList(QObject *parent = 0);
    ~LauncherApplicationsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
    void load();
    void insertBamfApplication(BamfApplication* bamf_application);
    QString desktopFilePathFromFavorite(QString favorite_id);
    QList<QLauncherApplication*> m_applications;

private slots:
    void onApplicationClosed();
    void onBamfViewOpened(BamfView* bamf_view);
};

#endif // LAUNCHERAPPLICATIONSLIST_H
