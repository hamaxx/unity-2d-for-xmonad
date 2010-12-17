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

    explicit QmlSpreadControl(QObject *parent = 0);
    ~QmlSpreadControl();

    unsigned long appId() const { return m_appId; }
    void setAppId(unsigned long appId);

    SpreadView *view() const { return m_view; }
    void setView(SpreadView *view) { m_view = view; }

    void doSpread() { emit activateSpread(); }

    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();

signals:
    void appIdChanged(unsigned long appId);
    void activateSpread();

private:
    unsigned long m_appId;
    SpreadView *m_view;
};

QML_DECLARE_TYPE(QmlSpreadControl)

#endif // QMLSPREADCONTROL_H
