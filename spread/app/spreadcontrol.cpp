#include <QDebug>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>
#include <QDBusConnection>

#include "spreadcontrol.h"
#include "spreadadaptor.h"

static const char* DBUS_SERVICE = "com.canonical.UnityQtSpread.Spread";
static const char* DBUS_OBJECT_PATH = "/Spread";

SpreadControl::SpreadControl(QObject *parent) :
    QObject(parent), mServiceWatcher(new QDBusServiceWatcher(this))
{
    mServiceWatcher->setConnection(QDBusConnection::sessionBus());
    mServiceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(mServiceWatcher, SIGNAL(serviceUnregistered(const QString&)), SLOT(slotServiceUnregistered(const QString&)));
}

SpreadControl::~SpreadControl() {
    QDBusConnection::sessionBus().unregisterService(mService);
}

bool SpreadControl::connectToBus(const QString& _service, const QString& _path)
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

void SpreadControl::SpreadAllWindows() {
    qDebug() << "DBUS: Received request to expose all windows";
    if (!m_qmlcontrol->inProgress()) {
        m_qmlcontrol->setAppId(0);
        m_qmlcontrol->doSpread();
    } else {
        qDebug() << "DBUS: Canceling current spread";
        m_qmlcontrol->doCancelSpread();
    }
}

void SpreadControl::SpreadApplicationWindows(unsigned int appId) {
    qDebug() << "DBUS: Received request to expose application windows of" << appId;
    if (!m_qmlcontrol->inProgress()) {
        m_qmlcontrol->setAppId(appId);
        m_qmlcontrol->doSpread();
    } else {
        qDebug() << "DBUS: Canceling current spread";
        m_qmlcontrol->doCancelSpread();
    }
}

void SpreadControl::slotServiceUnregistered(const QString& service)
{
    mServiceWatcher->removeWatchedService(service);
}

