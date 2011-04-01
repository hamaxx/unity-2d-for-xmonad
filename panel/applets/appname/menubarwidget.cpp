/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Self
#include "menubarwidget.h"

// Local
#include "config.h"
#include "debug_p.h"
#include "keyboardmodifiersmonitor.h"
#include "registrar.h"

// dbusmenu-qt
#include <dbusmenuimporter.h>

// bamf
#include <bamf-matcher.h>
#include <bamf-window.h>

// Qt
#include <QActionEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTimer>

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

MenuBarWidget::MenuBarWidget(QMenu* windowMenu, QWidget* parent)
: QWidget(parent)
, m_windowMenu(windowMenu)
{
    m_activeWinId = 0;
    setupRegistrar();
    setupMenuBar();

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*, BamfWindow*)),
        SLOT(slotActiveWindowChanged(BamfWindow*, BamfWindow*)));
    updateActiveWinId(BamfMatcher::get_default().active_window());
}

void MenuBarWidget::setupRegistrar()
{
    m_registrar = Registrar::instance();
    if (!m_registrar->connectToBus()) {
        UQ_WARNING << "could not connect registrar to DBus";
    }

    connect(m_registrar, SIGNAL(WindowRegistered(WId, const QString&, const QDBusObjectPath&)),
        SLOT(slotWindowRegistered(WId, const QString&, const QDBusObjectPath&)));
    connect(m_registrar, SIGNAL(WindowUnregistered(WId)),
        SLOT(slotWindowUnregistered(WId)));
}

void MenuBarWidget::setupMenuBar()
{
    QLabel* separatorLabel = new QLabel;
    QPixmap pix(unity2dDirectory() + "/panel/artwork/divider.png");
    separatorLabel->setPixmap(pix);
    separatorLabel->setFixedSize(pix.size());

    m_menuBar = new QMenuBar;
    new MenuBarClosedHelper(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(separatorLabel);
    layout->addWidget(m_menuBar);
    m_menuBar->setNativeMenuBar(false);

    m_updateMenuBarTimer = new QTimer(this);
    m_updateMenuBarTimer->setSingleShot(true);
    m_updateMenuBarTimer->setInterval(0);
    connect(m_updateMenuBarTimer, SIGNAL(timeout()),
        SLOT(updateMenuBar()));

    // Repaint the menubar when modifiers change so that the shortcut underline
    // is drawn or not
    connect(KeyboardModifiersMonitor::instance(), SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
        m_menuBar, SLOT(update()));
}

void MenuBarWidget::slotActiveWindowChanged(BamfWindow* /*former*/, BamfWindow* current)
{
    if (current) {
        updateActiveWinId(current);
    }
}

void MenuBarWidget::slotWindowRegistered(WId wid, const QString& service, const QDBusObjectPath& menuObjectPath)
{
    MyDBusMenuImporter* importer = new MyDBusMenuImporter(service, menuObjectPath.path(), this);
    delete m_importers.take(wid);
    m_importers.insert(wid, importer);
    connect(importer, SIGNAL(menuUpdated()), SLOT(slotMenuUpdated()));
    connect(importer, SIGNAL(actionActivationRequested(QAction*)), SLOT(slotActionActivationRequested(QAction*)));
    QMetaObject::invokeMethod(importer, "updateMenu", Qt::QueuedConnection);
}

void MenuBarWidget::slotWindowUnregistered(WId wid)
{
    MyDBusMenuImporter* importer = m_importers.take(wid);
    if (importer) {
        importer->deleteLater();
    }
    if (wid == m_activeWinId) {
        m_activeWinId = 0;
        updateMenuBar();
    }
}

void MenuBarWidget::slotMenuUpdated()
{
    DBusMenuImporter* importer = static_cast<DBusMenuImporter*>(sender());

    if (m_importers.value(m_activeWinId) == importer) {
        updateMenuBar();
    }
}

void MenuBarWidget::slotActionActivationRequested(QAction* action)
{
    DBusMenuImporter* importer = static_cast<DBusMenuImporter*>(sender());

    if (m_importers.value(m_activeWinId) == importer) {
        m_menuBar->setActiveAction(action);
    }
}

QMenu* MenuBarWidget::menuForWinId(WId wid) const
{
    MyDBusMenuImporter* importer = m_importers.value(wid);
    return importer ? importer->menu() : 0;
}

void MenuBarWidget::updateActiveWinId(BamfWindow* bamfWindow)
{
    WId id = bamfWindow ? bamfWindow->xid() : 0;
    if (id == m_activeWinId) {
        return;
    }
    if (id == window()->winId()) {
        // Do not update id if the active window is the one hosting this applet
        return;
    }
    m_activeWinId = id;
    updateMenuBar();
}

void MenuBarWidget::updateMenuBar()
{
    WId winId = m_activeWinId;
    QMenu* menu = menuForWinId(winId);

    if (!menu) {
        if (winId) {
            menu = m_windowMenu;
        } else {
            // No active window, show a desktop menubar
            // FIXME: Empty menu
            /*
            menu = mEmptyMenu;
            */
        }
    }

    m_menuBar->clear();
    // FIXME: Empty menu
    if (!menu) {
        return;
    }
    menu->installEventFilter(this);
    Q_FOREACH(QAction* action, menu->actions()) {
        if (action->isSeparator()) {
            continue;
        }
        m_menuBar->addAction(action);
    }
}

bool MenuBarWidget::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type()) {
    case QEvent::ActionAdded:
    case QEvent::ActionRemoved:
    case QEvent::ActionChanged:
        m_updateMenuBarTimer->start();
        break;
    default:
        break;
    }
    return false;
}

bool MenuBarWidget::isEmpty() const
{
    return m_menuBar->actions().isEmpty();
}

bool MenuBarWidget::isOpened() const
{
    return m_menuBar->activeAction();
}

// MenuBarClosedHelper ----------------------------------------
MenuBarClosedHelper::MenuBarClosedHelper(MenuBarWidget* widget)
: QObject(widget)
, m_widget(widget)
{
    widget->m_menuBar->installEventFilter(this);
}

bool MenuBarClosedHelper::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_widget->m_menuBar) {
        switch (event->type()) {
            case QEvent::ActionAdded:
            case QEvent::ActionRemoved:
            case QEvent::ActionChanged:
                menuBarActionEvent(static_cast<QActionEvent*>(event));
                break;
            default:
                break;
        }
    } else {
        // Top-level menus
        if (event->type() == QEvent::Hide) {
            // menu hide themselves when the menubar is closed but also when
            // one goes from one menu to another. The way to know this is to
            // check the value of QMenuBar::activeAction(), but at this point
            // it has not been updated yet, so we check in a delayed method.
            QMetaObject::invokeMethod(this, "emitMenuBarClosed", Qt::QueuedConnection);
        }
    }
    return false;
}

void MenuBarClosedHelper::emitMenuBarClosed()
{
    if (!m_widget->m_menuBar->activeAction()) {
        QMetaObject::invokeMethod(m_widget, "menuBarClosed");
    }
}

void MenuBarClosedHelper::menuBarActionEvent(QActionEvent* event)
{
    QMenu* menu = event->action()->menu();
    if (menu) {
        // Install/remove event filters on top level menus so that know when
        // they hide themselves and can emit menuBarClosed()
        switch (event->type()) {
        case QEvent::ActionAdded:
        case QEvent::ActionChanged:
            menu->installEventFilter(this);
            break;
        case QEvent::ActionRemoved:
            menu->removeEventFilter(this);
            break;
        default:
            break;
        }
    }

    // Emit isEmptyChanged() if necessary
    QList<QAction*> actions = m_widget->m_menuBar->actions();
    if (event->type() == QEvent::ActionAdded && actions.count() == 1) {
        QMetaObject::invokeMethod(m_widget, "isEmptyChanged");
    } else if (event->type() == QEvent::ActionRemoved && actions.isEmpty()) {
        QMetaObject::invokeMethod(m_widget, "isEmptyChanged");
    }
}

#include "menubarwidget.moc"
