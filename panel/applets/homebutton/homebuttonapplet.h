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
#ifndef HOMEBUTTONAPPLET_H
#define HOMEBUTTONAPPLET_H

// Local
#include <applet.h>

// Qt
#include <QToolButton>

class QDBusInterface;
class QDBusServiceWatcher;

class HomeButtonApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    HomeButtonApplet();
    ~HomeButtonApplet();

private Q_SLOTS:
    void slotButtonClicked();
    void dashActiveChanged(bool active);
    void serviceRegistered(QString name);

private:
    Q_DISABLE_COPY(HomeButtonApplet)
    QToolButton* m_button;
    QDBusInterface* m_dashInterface;
    QDBusServiceWatcher* m_serviceWatcher;
};

#endif /* HOMEBUTTONAPPLET_H */
