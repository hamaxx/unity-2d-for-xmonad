#ifndef BAMF_VIEW_H
#define BAMF_VIEW_H

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QDBusMetaType>

#include "qdbusconnectionqml.h"


class BamfView : public QDBusConnectionQML
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    inline bool active() const
    { return qvariant_cast<bool>(m_interface->property("Active")); }

    Q_PROPERTY(bool urgent READ urgent NOTIFY urgentChanged)
    inline bool urgent() const
    { return qvariant_cast<bool>(m_interface->property("Urgent")); }

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    inline bool running() const
    { return qvariant_cast<bool>(m_interface->property("Running")); }

    Q_PROPERTY(bool userVisible READ userVisible NOTIFY userVisibleChanged)
    inline bool userVisible() const
    { return qvariant_cast<bool>(m_interface->property("UserVisible")); }



public:
    BamfView(QObject *parent = 0) : QDBusConnectionQML(parent)
    {

        QObject::connect(this, SIGNAL(objectPathChanged(QString)), this, SLOT(connectSignals()));
        QObject::connect(this, SIGNAL(serviceChanged(QString)), this, SLOT(connectSignals()));
    }

    ~BamfView()
    {
    }

    Q_SLOT void connectSignals()
    {
        // FIXME: missing disconnection
        if(m_service.isEmpty() || m_objectPath.isEmpty())
            return;

        QObject::connect(m_interface, SIGNAL(ActiveChanged(bool)), this, SIGNAL(activeChanged(bool)));
        QObject::connect(m_interface, SIGNAL(Closed()), this, SIGNAL(closed()));
        QObject::connect(m_interface, SIGNAL(ChildAdded(QString)), this, SIGNAL(childAdded(QString)));
        QObject::connect(m_interface, SIGNAL(ChildRemoved(QString)), this, SIGNAL(childRemoved(QString)));
        QObject::connect(m_interface, SIGNAL(RunningChanged(bool)), this, SIGNAL(runningChanged(bool)));
        QObject::connect(m_interface, SIGNAL(UrgentChanged(bool)), this, SIGNAL(urgentChanged(bool)));
        QObject::connect(m_interface, SIGNAL(UserVisibleChanged(bool)), this, SIGNAL(userVisibleChanged(bool)));
    }

    Q_INVOKABLE inline QStringList Children()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "Children", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QStringList Parents()
    {
        if (m_interface == NULL)
            return QStringList();

        QList<QVariant> argumentList;

        QDBusReply<QStringList> reply = m_interface->callWithArgumentList(QDBus::Block, "Parents", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QStringList();
        }
      return reply.value();
    }
    Q_INVOKABLE inline bool IsActive()
    {
        if (m_interface == NULL)
            return bool();

        QList<QVariant> argumentList;

        QDBusReply<bool> reply = m_interface->callWithArgumentList(QDBus::Block, "IsActive", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return bool();
        }
      return reply.value();
    }
    Q_INVOKABLE inline bool IsRunning()
    {
        if (m_interface == NULL)
            return bool();

        QList<QVariant> argumentList;

        QDBusReply<bool> reply = m_interface->callWithArgumentList(QDBus::Block, "IsRunning", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return bool();
        }
      return reply.value();
    }
    Q_INVOKABLE inline bool IsUrgent()
    {
        if (m_interface == NULL)
            return bool();

        QList<QVariant> argumentList;

        QDBusReply<bool> reply = m_interface->callWithArgumentList(QDBus::Block, "IsUrgent", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return bool();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString Name()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "Name", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString Icon()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "Icon", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }
    Q_INVOKABLE inline bool UserVisible()
    {
        if (m_interface == NULL)
            return bool();

        QList<QVariant> argumentList;

        QDBusReply<bool> reply = m_interface->callWithArgumentList(QDBus::Block, "UserVisible", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return bool();
        }
      return reply.value();
    }
    Q_INVOKABLE inline QString ViewType()
    {
        if (m_interface == NULL)
            return QString();

        QList<QVariant> argumentList;

        QDBusReply<QString> reply = m_interface->callWithArgumentList(QDBus::Block, "ViewType", argumentList);
        if (!reply.isValid())
        {
            qWarning() << reply.error();
            return QString();
        }
      return reply.value();
    }

Q_SIGNALS:
    void activeChanged(bool);
    void closed();
    void childAdded(QString);
    void childRemoved(QString);
    void runningChanged(bool);
    void urgentChanged(bool);
    void userVisibleChanged(bool);
};
#endif
