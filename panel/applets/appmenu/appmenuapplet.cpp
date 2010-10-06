/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "appmenuapplet.h"

// Local
#include "debug_p.h"
#include "registrar.h"

// dbusmenu-qt
#include <dbusmenuimporter.h>

// bamf
#include <bamf-matcher.h>
#include <bamf-window.h>

// Qt
#include <QHBoxLayout>
#include <QMenuBar>

class MyDBusMenuImporter : public DBusMenuImporter
{
public:
    MyDBusMenuImporter(const QString &service, const QString &path, QObject *parent)
    : DBusMenuImporter(service, path, parent)
    , m_service(service)
    , m_path(path)
    {}

    QString service() const { return m_service; }
    QString path() const { return m_path; }

private:
    QString m_service;
    QString m_path;
};

AppMenuApplet::AppMenuApplet()
{
    m_activeWinId = 0;
    setupRegistrar();
    setupMenuBar();

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*, BamfWindow*)),
        SLOT(slotActiveWindowChanged(BamfWindow*, BamfWindow*)));
    updateActiveWinId(BamfMatcher::get_default().active_window());
}

void AppMenuApplet::setupRegistrar()
{
    m_registrar = new Registrar(this);
    if (!m_registrar->connectToBus()) {
        UQ_WARNING << "could not connect registrar to DBus";
    }

    connect(m_registrar, SIGNAL(WindowRegistered(WId, const QString&, const QDBusObjectPath&)),
        SLOT(slotWindowRegistered(WId, const QString&, const QDBusObjectPath&)));
}

void AppMenuApplet::setupMenuBar()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    m_menuBar = new QMenuBar;
    layout->addWidget(m_menuBar);
    m_menuBar->setNativeMenuBar(false);
    m_menuBar->addMenu("File");
}

void AppMenuApplet::slotActiveWindowChanged(BamfWindow* /*former*/, BamfWindow* current)
{
    if (current) {
        updateActiveWinId(current);
    }
}

void AppMenuApplet::slotWindowRegistered(WId wid, const QString& service, const QDBusObjectPath& menuObjectPath)
{
    MyDBusMenuImporter* importer = new MyDBusMenuImporter(service, menuObjectPath.path(), this);
    delete m_importers.take(wid);
    m_importers.insert(wid, importer);
    connect(importer, SIGNAL(menuUpdated()), SLOT(slotMenuUpdated()));
    connect(importer, SIGNAL(actionActivationRequested(QAction*)), SLOT(slotActionActivationRequested(QAction*)));
    QMetaObject::invokeMethod(importer, "updateMenu", Qt::QueuedConnection);
}

void AppMenuApplet::slotMenuUpdated()
{
    DBusMenuImporter* importer = static_cast<DBusMenuImporter*>(sender());

    if (m_importers.value(m_activeWinId) == importer) {
        updateMenuBar();
    }
}

void AppMenuApplet::slotActionActivationRequested(QAction* action)
{
    DBusMenuImporter* importer = static_cast<DBusMenuImporter*>(sender());

    if (m_importers.value(m_activeWinId) == importer) {
        m_menuBar->setActiveAction(action);
    }
}

QMenu* AppMenuApplet::menuForWinId(WId wid) const
{
    MyDBusMenuImporter* importer = m_importers.value(wid);
    return importer ? importer->menu() : 0;
}

void AppMenuApplet::updateActiveWinId(BamfWindow* window)
{
    WId id = window ? window->xid() : 0;
    if (id == m_activeWinId) {
        return;
    }
    UQ_VAR(id);
    UQ_VAR(this->window()->winId());
    if (id == this->window()->winId()) {
        // Do not update id if the active window is the one hosting this applet
        UQ_DEBUG << "No update";
        return;
    }
    m_activeWinId = id;
    updateMenuBar();
}

void AppMenuApplet::updateMenuBar()
{
    UQ_DEBUG;
    WId winId = m_activeWinId;
    QMenu* menu = menuForWinId(winId);

    if (!menu) {
        if (winId) {
            // We have an active window
            // FIXME: transient check
            /*
            WId mainWinId = KWindowSystem::transientFor(winId);
            if (mainWinId) {
                // We have a parent window, use a disabled version of its
                // menubar if it has one.
                QMenu* mainMenu = menuForWinId(mainWinId);
                if (mainMenu) {
                    mMenuCloner->setOriginalMenu(mainMenu);
                    menu = mMenuCloner->clonedMenu();
                }
            }*/
            // FIXME: WindowMenuManager
            /*
            if (!menu) {
                // No suitable menubar but we have a window, use the
                // generic window menu
                mWindowMenuManager->setWinId(winId);
                menu = mWindowMenu;
            }
            */
        } else {
            // No active window, show a desktop menubar
            // FIXME: Empty menu
            /*
            menu = mEmptyMenu;
            */
        }
    }
    UQ_VAR(menu);
    fillMenuBar(menu);
}

void AppMenuApplet::fillMenuBar(QMenu* menu)
{
    m_menuBar->clear();
    // FIXME: Empty menu
    if (!menu) {
        return;
    }
    Q_FOREACH(QAction* action, menu->actions()) {
        if (action->isSeparator()) {
            continue;
        }
        m_menuBar->addAction(action);
    }
}

#include "appmenuapplet.moc"
