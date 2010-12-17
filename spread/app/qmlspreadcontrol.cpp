#include <QDebug>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>
#include <QDBusConnection>

#include "qmlspreadcontrol.h"
#include "spreadview.h"

QmlSpreadControl::QmlSpreadControl(QObject *parent) :
    QObject(parent), m_appId(0), m_inProgress(false) {
}

QmlSpreadControl::~QmlSpreadControl() {
}

void QmlSpreadControl::setAppId(unsigned long appId) {
    if (m_appId != appId) {
        m_appId = appId;
        emit appIdChanged(appId);
    }
}

void QmlSpreadControl::show() {
    if (m_view == 0) return;
    m_view->showMaximized();
}

void QmlSpreadControl::hide() {
    if (m_view == 0) return;
    m_view->hide();
}
