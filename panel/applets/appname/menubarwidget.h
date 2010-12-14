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

class MyDBusMenuImporter;
class Registrar;

typedef QHash<WId, MyDBusMenuImporter*> ImporterForWId;

class MenuBarWidget : public QWidget
{
Q_OBJECT
public:
    MenuBarWidget(QMenu* windowMenu, QWidget* parent = 0);

    QMenuBar* menuBar() const { return m_menuBar; }

Q_SIGNALS:
    void menuBarClosed();

protected:
    bool eventFilter(QObject*, QEvent*); // reimp

private Q_SLOTS:
    void slotActiveWindowChanged(BamfWindow*, BamfWindow*);
    void slotWindowRegistered(WId, const QString& service, const QDBusObjectPath& menuObjectPath);
    void slotMenuUpdated();
    void slotActionActivationRequested(QAction* action);
    void emitMenuBarClosed();

private:
    Q_DISABLE_COPY(MenuBarWidget)

    QMenuBar* m_menuBar;
    Registrar* m_registrar;
    ImporterForWId m_importers;
    WId m_activeWinId;
    QMenu* m_windowMenu;

    void setupRegistrar();
    void setupMenuBar();
    QMenu* menuForWinId(WId) const;
    void updateActiveWinId(BamfWindow*);
    void updateMenuBar();
    void fillMenuBar(QMenu*);
    void menuBarActionEvent(QActionEvent*);
};

#endif /* MENUBARWIDGET_H */
