#ifndef LAUNCHERAPPLICATIONSLIST_H
#define LAUNCHERAPPLICATIONSLIST_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QtDeclarative/qdeclarative.h>

class LauncherApplication;
class BamfApplication;
class BamfView;
class GConfItemQmlWrapper;

class LauncherApplicationsList : public QAbstractListModel
{
    Q_OBJECT
public:
    LauncherApplicationsList(QObject *parent = 0);
    ~LauncherApplicationsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Q_INVOKABLE void insertFavoriteApplication(QString desktop_file);

private:
    void load();
    void insertBamfApplication(BamfApplication* bamf_application);

    void insertApplication(LauncherApplication* application);
    void removeApplication(LauncherApplication* application);

    static QString desktopFilePathFromFavorite(QString favorite_id);
    static QString favoriteFromDesktopFilePath(QString desktop_file);

    void addApplicationToFavorites(LauncherApplication* application);
    void removeApplicationFromFavorites(LauncherApplication* application);

    /* List of LauncherApplication displayed in the launcher. */
    QList<LauncherApplication*> m_applications;
    /* Hash of desktop file names to LauncherApplication used to reduce
       the algorithmical complexity of merging the list of running applications
       and the list of favorited applications into the list of applications
       displayed (m_applications).
    */
    QHash<QString, LauncherApplication*> m_desktop_files;

    GConfItemQmlWrapper* m_favorites_list;

private slots:
    void onApplicationClosed();
    void onBamfViewOpened(BamfView* bamf_view);
    void onApplicationStickyChanged(bool sticky);
};

QML_DECLARE_TYPE(LauncherApplicationsList)

#endif // LAUNCHERAPPLICATIONSLIST_H
