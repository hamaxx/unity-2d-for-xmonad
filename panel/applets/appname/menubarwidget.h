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

#ifndef MENUBARWIDGET_H
#define MENUBARWIDGET_H

// Qt
#include <QHash>
#include <QWidget>

class BamfWindow;

class QActionEvent;
class QDBusObjectPath;
class QMenu;
class QMenuBar;
class QTimer;

class MyDBusMenuImporter;
class Registrar;

typedef QHash<WId, MyDBusMenuImporter*> ImporterForWId;

class MenuBarWidget;

/**
 * An helper class which monitors the menubar and emits MenuBarWidget::menuBarClosed()
 * to indicate if the menu was closed or updated.
 */
class MenuBarClosedHelper : public QObject
{
Q_OBJECT
public:
    MenuBarClosedHelper(MenuBarWidget*);

Q_SIGNALS:
    void menuBarClosed();

protected:
    bool eventFilter(QObject*, QEvent*); //reimp

private Q_SLOTS:
    void emitMenuBarClosed();

private:
    MenuBarWidget* m_widget;
    void menuBarActionEvent(QActionEvent*);
};

class MenuBarWidget : public QWidget
{
Q_OBJECT
public:
    MenuBarWidget(QMenu* windowMenu, QWidget* parent = 0);

    bool isEmpty() const;
    bool isOpened() const;

Q_SIGNALS:
    void menuBarChanged();
    void isEmptyChanged();

protected:
    bool eventFilter(QObject*, QEvent*); // reimp

private Q_SLOTS:
    void slotActiveWindowChanged(BamfWindow*, BamfWindow*);
    void slotViewOpened();
    void slotWindowRegistered(WId, const QString& service, const QDBusObjectPath& menuObjectPath);
    void slotWindowUnregistered(WId);
    void slotMenuUpdated();
    void slotActionActivationRequested(QAction* action);
    void slotMenuBarClosed();
    void updateMenuBar();

private:
    Q_DISABLE_COPY(MenuBarWidget)

    QMenuBar* m_menuBar;
    MenuBarClosedHelper* m_menuBarMonitor;
    Registrar* m_registrar;
    ImporterForWId m_importers;
    WId m_activeWinId;
    QMenu* m_windowMenu;
    QTimer* m_updateMenuBarTimer;
    bool m_isMenuOpen;

    void setupRegistrar();
    void setupMenuBar();
    QMenu* menuForWinId(WId) const;
    void updateActiveWinId(BamfWindow*);

    friend class MenuBarClosedHelper;
};

#endif /* MENUBARWIDGET_H */
