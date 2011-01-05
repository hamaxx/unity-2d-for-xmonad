/*
 * Plasma applet to display application window menus
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef REGISTRAR_H
#define REGISTRAR_H

// Qt
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QObject>
#include <QWidget> // For WId

class QDBusObjectPath;
class QDBusServiceWatcher;
class QMenu;

struct MenuInfo
{
    MenuInfo()
    : winId(0)
    , path("/")
    {}

    uint winId;
    QString service;
    QDBusObjectPath path;
};
Q_DECLARE_METATYPE(MenuInfo)

typedef QList<MenuInfo> MenuInfoList;
Q_DECLARE_METATYPE(MenuInfoList)

class Registrar : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    Registrar(QObject*);
    ~Registrar();

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());

Q_SIGNALS:
    void WindowRegistered(WId wid, const QString& service, const QDBusObjectPath&);
    void WindowUnregistered(WId wid);

public Q_SLOTS:
    Q_NOREPLY void RegisterWindow(WId wid, const QDBusObjectPath& menuObjectPath);
    Q_NOREPLY void UnregisterWindow(WId wid);
    QString GetMenuForWindow(WId wid, QDBusObjectPath& menuObjectPath);
    MenuInfoList GetMenus();

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

private:
    QDBusServiceWatcher* mServiceWatcher;
    typedef QHash<WId, MenuInfo> MenuInfoDb;
    MenuInfoDb mDb;
    QString mService;
};

#endif /* REGISTRAR_H */
