#ifndef LAUNCHERAPPLICATIONSLIST_H
#define LAUNCHERAPPLICATIONSLIST_H

#include "launcherapplication.h"

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QString>
#include <QObject>

#include "gconfitem-qml-wrapper.h"

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
    static QString desktopFilePathFromFavorite(QString favorite_id);
    static QString favoriteFromDesktopFilePath(QString desktop_file);
    void monitorApplicationStickiness(QLauncherApplication* application);
    bool isApplicationInFavorites(QString desktop_file);
    void addApplicationToFavorites(QLauncherApplication* application);
    void removeApplicationFromFavorites(QString desktop_file);

    QList<QLauncherApplication*> m_applications;
    GConfItemQmlWrapper* m_favorites_list;

private slots:
    void onFavoritesListChanged();
    void onApplicationClosed();
    void onBamfViewOpened(BamfView* bamf_view);
    void onApplicationStickyChanged(bool sticky);
};

#endif // LAUNCHERAPPLICATIONSLIST_H
