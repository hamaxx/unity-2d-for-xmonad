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
// Local

// Unity
#include <appletcontainer.h>
#include <panel.h>

// Qt
#include <QApplication>

using namespace UnityQt;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Panel panel;
    panel.setEdge(Panel::TopEdge);
    panel.setFixedHeight(24);
    panel.addWidget(new AppletContainer("appname"));
    panel.addWidget(new AppletContainer("appmenu"));
    panel.addSpacer();
    panel.addWidget(new AppletContainer("appindicator"));
    panel.addWidget(new AppletContainer("indicator"));
    panel.show();
    return app.exec();
}
