/*
 * This file is part of unity-2d
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

class QX11EmbedContainer;

struct _IndicatorPlugin;

class IndicatorApplet : public Unity2d::Applet
{
Q_OBJECT
public:
    IndicatorApplet();

private Q_SLOTS:
    void loadIndicators();
    void slotActionAdded(QAction*);
    void slotActionRemoved(QAction*);
    void createGtkIndicator();
    void adjustGtkIndicatorSize();

private:
    Q_DISABLE_COPY(IndicatorApplet)

    QDBusInterface* m_watcher;
    QMenuBar* m_menuBar;
    QX11EmbedContainer* m_container;
    struct _IndicatorPlugin* m_gtkIndicator;

    void setupUi();
};

#endif /* INDICATORAPPLET_H */
