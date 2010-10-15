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
#include <appindicator/appindicatorapplet.h>
#include <appmenu/appmenuapplet.h>
#include <appname/appnameapplet.h>
#include <homebutton/homebuttonapplet.h>
#include <indicator/indicatorapplet.h>

// Unity
#include <panel.h>

// Qt
#include <QApplication>

using namespace UnityQt;

QPalette getPalette()
{
    QPalette palette;

    QBrush bg(QPixmap("/usr/share/unity/themes/panel_background.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);
    return palette;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Panel panel;
    panel.setEdge(Panel::TopEdge);
    panel.setPalette(getPalette());
    panel.setFixedHeight(24);

    panel.addWidget(new HomeButtonApplet);
    panel.addWidget(new AppNameApplet);
    panel.addWidget(new AppMenuApplet);
    panel.addSpacer();
    panel.addWidget(new AppIndicatorApplet);
    panel.addWidget(new IndicatorApplet);
    panel.show();
    return app.exec();
}
