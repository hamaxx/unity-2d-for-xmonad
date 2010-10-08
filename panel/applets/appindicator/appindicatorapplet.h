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
#ifndef APPINDICATORAPPLET_H
#define APPINDICATORAPPLET_H

// Local
#include <applet.h>

// Qt
#include <QDBusInterface>
#include <QHBoxLayout>

class AppIndicatorApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    AppIndicatorApplet();

private:
    Q_DISABLE_COPY(AppIndicatorApplet)

    QDBusInterface* m_watcher;
    QHBoxLayout* m_layout;

    void setupDBus();
    void setupUi();
    void createItems();
    void createItem(const QString&);
};

APPLET_MAIN(AppIndicatorApplet)

#endif /* APPINDICATORAPPLET_H */
