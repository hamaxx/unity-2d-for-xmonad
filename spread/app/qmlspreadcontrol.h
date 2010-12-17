#ifndef QMLSPREADCONTROL_H
#define QMLSPREADCONTROL_H

#include <QObject>
#include <QtDeclarative/qdeclarative.h>

class SpreadView;

class QmlSpreadControl : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(unsigned long appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(bool inProgress READ inProgress WRITE setInProgress)

    explicit QmlSpreadControl(QObject *parent = 0);
    ~QmlSpreadControl();

    unsigned long appId() const { return m_appId; }
    void setAppId(unsigned long appId);

    SpreadView *view() const { return m_view; }
    void setView(SpreadView *view) { m_view = view; }

    bool inProgress() const { return m_inProgress; }
    void setInProgress(bool inProgress) { m_inProgress = inProgress; }

    void doSpread() { emit activateSpread(); }
    void doCancelSpread() { emit cancelSpread(); }

    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();

signals:
    void appIdChanged(unsigned long appId);
    void activateSpread();
    void cancelSpread();

private:
    unsigned long m_appId;
    SpreadView *m_view;
    bool m_inProgress;
};

QML_DECLARE_TYPE(QmlSpreadControl)

#endif // QMLSPREADCONTROL_H
