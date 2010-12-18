#ifndef SPREADCONTROL_H
#define SPREADCONTROL_H

#include <QObject>
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QtDeclarative/qdeclarative.h>

class QDBusObjectPath;
class QDBusServiceWatcher;

class SpreadControl : public QObject, protected QDBusContext
{
    Q_OBJECT

    Q_PROPERTY(unsigned long appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(bool inProgress READ inProgress WRITE setInProgress)

public:
    explicit SpreadControl(QObject *parent = 0);
    ~SpreadControl();

    unsigned long appId() const { return m_appId; }
    void setAppId(unsigned long appId);

    bool inProgress() const { return m_inProgress; }
    void setInProgress(bool inProgress) { m_inProgress = inProgress; }

    void doSpread() { emit activateSpread(); }
    void doCancelSpread() { emit cancelSpread(); }

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());

public Q_SLOTS:
    Q_NOREPLY void SpreadAllWindows();
    Q_NOREPLY void SpreadApplicationWindows(unsigned int appId);

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

Q_SIGNALS:
    void appIdChanged(unsigned long appId);
    void activateSpread();
    void cancelSpread();

private:
    QDBusServiceWatcher* mServiceWatcher;
    QString mService;
    unsigned long m_appId;
    bool m_inProgress;
};

QML_DECLARE_TYPE(SpreadControl)

#endif // SPREADCONTROL_H
