#ifndef SPREADCONTROL_H
#define SPREADCONTROL_H

#include <QObject>
#include <QDBusContext>
#include <QDBusObjectPath>

#include "qmlspreadcontrol.h"

class QDBusObjectPath;
class QDBusServiceWatcher;

class SpreadControl : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit SpreadControl(QObject *parent = 0);
    ~SpreadControl();

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());
    void setQmlControl(QmlSpreadControl *control) { m_qmlcontrol = control; }

public Q_SLOTS:
    Q_NOREPLY void SpreadAllWindows();
    Q_NOREPLY void SpreadApplicationWindows(unsigned int appId);

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

private:
    QDBusServiceWatcher* mServiceWatcher;
    QString mService;
    QmlSpreadControl* m_qmlcontrol;
};

#endif // SPREADCONTROL_H
