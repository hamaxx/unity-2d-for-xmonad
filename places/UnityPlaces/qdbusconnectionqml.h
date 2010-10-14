#ifndef QDBUSCONNECTIONQML_H
#define QDBUSCONNECTIONQML_H

#include <QObject>
#include <QDBusInterface>

class QDBusConnectionQML : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:
    QDBusConnectionQML(QObject *parent = 0);
    ~QDBusConnectionQML();

    /* getters */
    QString objectPath() const;
    QString service() const;

    /* setters */
    void setObjectPath(QString objectPath);
    void setService(QString service);

private slots:
    void connectToDBus();

signals:
    void objectPathChanged(QString);
    void serviceChanged(QString);

protected:
    QString m_service;
    QString m_objectPath;
    QDBusInterface* m_interface;
};

#endif

