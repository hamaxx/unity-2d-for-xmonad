#ifndef BAMF_MATCHER_H
#define BAMF_MATCHER_H

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QDBusMetaType>

#include "qdbusconnectionqml.h"


typedef QList<uint> QListuint;
Q_DECLARE_METATYPE(QListuint)

class BamfMatcher : public QDBusConnectionQML
{
    Q_OBJECT



public:
    BamfMatcher(QObject *parent = 0) : QDBusConnectionQML(parent)
    {
        qDBusRegisterMetaType<QListuint>();

        QObject::connect(this, SIGNAL(objectPathChanged(QString)), this, SLOT(connectSignals()));
        QObject::connect(this, SIGNAL(serviceChanged(QString)), this, SLOT(connectSignals()));
    }

    ~BamfMatcher()
    {
    }

    Q_SLOT void connectSignals()
    {
        // FIXME: missing disconnection
        if(m_service.isEmpty() || m_objectPath.isEmpty())
            return;

        QObject::connect(m_interface, SIGNAL(ActiveApplicationChanged(QString, QString)), this, SIGNAL(activeApplicationChanged(QString, QString)));
        QObject::connect(m_interface, SIGNAL(ActiveWindowChanged(QString, QString)), this, SIGNAL(activeWindowChanged(QString, QString)));
        QObject::connect(m_interface, SIGNAL(ViewClosed(QString, QString)), this, SIGNAL(viewClosed(QString, QString)));
        QObject::connect(m_interface, SIGNAL(ViewOpened(QString, QString)), this, SIGNAL(viewOpened(QString, QString)));
    }

    Q_INVOKABLE inline QListuint XidsForApplication(QString application)
    {
        if (m_interface == NULL)
            return QListuint();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(application);

        QDBusReply<QListuint> reply = m_interface->callWithArgumentList(QDBus::Block, "XidsForApplication", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QListuint();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QStringList TabPaths()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "TabPaths", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QStringList RunningApplications()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "RunningApplications", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline void RegisterFavorites(QStringList favorites)
    {
        if (m_interface == NULL)
            return;

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(favorites);

        QDBusReply<void> reply = m_interface->callWithArgumentList(QDBus::Block, "RegisterFavorites", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return;
        }
    }
    Q_INVOKABLE inline QString PathForApplication(QString applicaiton)
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(applicaiton);

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "PathForApplication", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QStringList WindowPaths()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "WindowPaths", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QStringList ApplicationPaths()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "ApplicationPaths", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline bool ApplicationIsRunning(QString application)
    {
        if (m_interface == NULL)
            return bool();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(application);

        QDBusReply<bool> reply = m_interface->callWithArgumentList(QDBus::Block, "ApplicationIsRunning", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return bool();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString ApplicationForXid(uint xid)
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(xid);

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "ApplicationForXid", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString ActiveWindow()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "ActiveWindow", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString ActiveApplication()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "ActiveApplication", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }

Q_SIGNALS:
    void activeApplicationChanged(QString, QString);
    void activeWindowChanged(QString, QString);
    void viewClosed(QString, QString);
    void viewOpened(QString, QString);
};
#endif
