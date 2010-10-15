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
#ifndef APPMENUAPPLET_H
#define APPMENUAPPLET_H

// Local
#include <applet.h>

// Qt
#include <QHash>

class BamfWindow;

class QDBusObjectPath;
class QMenu;
class QMenuBar;

class MyDBusMenuImporter;
class Registrar;

typedef QHash<WId, MyDBusMenuImporter*> ImporterForWId;

class AppMenuApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    AppMenuApplet();

private Q_SLOTS:
    void slotActiveWindowChanged(BamfWindow*, BamfWindow*);
    void slotWindowRegistered(WId, const QString& service, const QDBusObjectPath& menuObjectPath);
    void slotMenuUpdated();
    void slotActionActivationRequested(QAction* action);

private:
    Q_DISABLE_COPY(AppMenuApplet)

    QMenuBar* m_menuBar;
    Registrar* m_registrar;
    ImporterForWId m_importers;
    WId m_activeWinId;

    void setupRegistrar();
    void setupMenuBar();
    QMenu* menuForWinId(WId) const;
    void updateActiveWinId(BamfWindow*);
    void updateMenuBar();
    void fillMenuBar(QMenu*);
};

#endif /* APPMENUAPPLET_H */
