#include <QDebug>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>
#include <QDBusConnection>

#include "dbusproxy.h"
#include "spreadadaptor.h"
#include "spreadview.h"

static const char* DBUS_SERVICE = "com.canonical.UnityQtSpread.Spread";
static const char* DBUS_OBJECT_PATH = "/com/canonical/UnityQtSpread/Spread";

DBusProxy::DBusProxy(QObject *parent) :
    QObject(parent), m_appId(0),
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

bool DBusProxy::connectToBus(const QString& _service, const QString& _path)
{
    qDebug() << _service << " , " << _path;
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
    qDebug() << "DBUS: Received request to expose all windows";
    setAppId(0);
    emit activateSpread();
}

void DBusProxy::SpreadApplicationWindows(unsigned int appId) {
    qDebug() << "DBUS: Received request to expose application windows of" << appId;
    setAppId(appId);
    emit activateSpread();
}

void DBusProxy::slotServiceUnregistered(const QString& service)
{
    mServiceWatcher->removeWatchedService(service);
}

void DBusProxy::show() {
    if (m_view == 0) return;
    m_view->showMaximized();
}

void DBusProxy::hide() {
    if (m_view == 0) return;
    m_view->hide();
}
