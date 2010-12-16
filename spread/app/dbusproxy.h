#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QWidget> // for WId

class QDBusObjectPath;
class QDBusServiceWatcher;
class QMenu;

class DBusProxy : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_ENUMS(SpreadType)

public:
    enum SpreadType {
        SpreadNone,
        SpreadAll,
        SpreadApplication
    };

    Q_PROPERTY(SpreadType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(WId appId READ appId WRITE setAppId NOTIFY appIdChanged)

    explicit DBusProxy(QObject *parent = 0);
    ~DBusProxy();

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());

    WId appId() const { return m_appId; }
    void setAppId(WId appId);
    SpreadType type() const { return m_type; }
    void setType(SpreadType type);

signals:
    void appIdChanged(WId appId);
    void typeChanged(SpreadType type);
    void activateSpread();

public Q_SLOTS:
    Q_NOREPLY void SpreadAllWindows();
    Q_NOREPLY void SpreadApplicationWindows(unsigned int appId);

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

private:
    SpreadType m_type;
    WId m_appId;
    QDBusServiceWatcher* mServiceWatcher;
    QString mService;
};

QML_DECLARE_TYPE(DBusProxy)

#endif // DBUSPROXY_H
