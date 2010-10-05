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
    void insertFavoriteApplication(QString desktop_file);

    QLauncherApplication* insertApplication(QString desktop_file);
    void removeApplication(QString desktop_file);

    static QString desktopFilePathFromFavorite(QString favorite_id);
    static QString favoriteFromDesktopFilePath(QString desktop_file);

    void addApplicationToFavorites(QString desktop_file);
    void removeApplicationFromFavorites(QString desktop_file);

    /* In order to reduce algorithmical complexity when updating and
       rendering the list of applications as well as overall code
       complexity a 2 level structure was chosen to store applications.
       A list of desktop file names ordered by placement in the launcher,
       also called priority is used for rendering.
       A hash of desktop file names to QLauncherApplication is used for
       modifications and presence checking.
    */
    QList<QString> m_desktop_files;
    QHash<QString, QLauncherApplication*> m_applications;

    GConfItemQmlWrapper m_favorites_list;

private slots:
    void onApplicationClosed();
    void onBamfViewOpened(BamfView* bamf_view);
    void onApplicationStickyChanged(bool sticky);
};

#endif // LAUNCHERAPPLICATIONSLIST_H
