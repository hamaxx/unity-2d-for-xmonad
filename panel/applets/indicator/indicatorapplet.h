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
#ifndef INDICATORAPPLET_H
#define INDICATORAPPLET_H

// Local
#include <applet.h>

// Qt
#include <QDBusInterface>
#include <QMenuBar>

class IndicatorApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    IndicatorApplet();

private:
    Q_DISABLE_COPY(IndicatorApplet)

    QDBusInterface* m_watcher;
    QMenuBar* m_menuBar;

    void setupUi();
    void loadIndicators();
};

APPLET_MAIN(IndicatorApplet)

#endif /* INDICATORAPPLET_H */
