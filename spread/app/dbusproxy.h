#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <QDBusContext>
#include <QDBusObjectPath>

class QDBusObjectPath;
class QDBusServiceWatcher;
class SpreadView;

class DBusProxy : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    Q_PROPERTY(unsigned long appId READ appId WRITE setAppId NOTIFY appIdChanged)

    explicit DBusProxy(QObject *parent = 0);
    ~DBusProxy();

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());

    unsigned long appId() const { return m_appId; }
    void setAppId(unsigned long appId);

    SpreadView *view() const { return m_view; }
    void setView(SpreadView *view) { m_view = view; }

    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();

signals:
    void appIdChanged(unsigned long appId);
    void activateSpread();

public Q_SLOTS:
    Q_NOREPLY void SpreadAllWindows();
    Q_NOREPLY void SpreadApplicationWindows(unsigned int appId);

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

private:
    unsigned long m_appId;
    QDBusServiceWatcher* mServiceWatcher;
    QString mService;
    SpreadView *m_view;
};

QML_DECLARE_TYPE(DBusProxy)

#endif // DBUSPROXY_H
