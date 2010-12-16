#include <QDebug>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>
#include <QDBusConnection>

#include "dbusproxy.h"
#include "spreadadaptor.h"

static const char* DBUS_SERVICE = "com.canonical.UnityQtSpread.Spread";
static const char* DBUS_OBJECT_PATH = "/com/canonical/UnityQtSpread/Spread";

DBusProxy::DBusProxy(QObject *parent) :
    QObject(parent), m_type(DBusProxy::SpreadNone), m_appId(0),
    mServiceWatcher(new QDBusServiceWatcher(this))
{
    mServiceWatcher->setConnection(QDBusConnection::sessionBus());
    mServiceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(mServiceWatcher, SIGNAL(serviceUnregistered(const QString&)), SLOT(slotServiceUnregistered(const QString&)));
}

DBusProxy::~DBusProxy() {
    QDBusConnection::sessionBus().unregisterService(mService);
}

void DBusProxy::setAppId(unsigned long appId) {
    if (m_appId != appId) {
        m_appId = appId;
        emit appIdChanged(appId);
    }
}

void DBusProxy::setType(SpreadType type) {
    if (m_type != type) {
        m_type = type;
        emit typeChanged(type);
    }
}

bool DBusProxy::connectToBus(const QString& _service, const QString& _path)
{
    mService = _service.isEmpty() ? DBUS_SERVICE : _service;
    QString path = _path.isEmpty() ? DBUS_OBJECT_PATH : _path;

    bool ok = QDBusConnection::sessionBus().registerService(mService);
    if (!ok) {
        return false;
    }
    new SpreadAdaptor(this);
    QDBusConnection::sessionBus().registerObject(path, this);

    return true;
}

void DBusProxy::SpreadAllWindows() {
    setAppId(0);
    setType(SpreadAll);
    emit activateSpread();
}

void DBusProxy::SpreadApplicationWindows(unsigned int appId) {
    setAppId(appId);
    setType(SpreadApplication);
    emit activateSpread();
}

void DBusProxy::slotServiceUnregistered(const QString& service)
{
    mServiceWatcher->removeWatchedService(service);
}
