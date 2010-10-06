/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: LGPL v3
 */
// Self
#include "applet.h"

// Local

// Qt
#include <QApplication>

// System
#include <iostream>

namespace UnityQt
{

struct AppletPrivate
{
};

Applet::Applet()
: QX11EmbedWidget()
, d(new AppletPrivate)
{
}

Applet::~Applet()
{
    delete d;
}

int appletMain(int argc, char** argv, AppletCreatorFunction creator)
{
    QApplication app(argc, argv);
    Applet* applet = creator();
    if (argc == 1) {
        std::cerr << "Running applet in standalone mode" << std::endl;
    } else if (argc == 2) {
        bool ok;
        WId wid = QString(argv[1]).toInt(&ok);
        if (!ok) {
            std::cerr << "Window id " << argv[1] << " is not a number" << std::endl;
            return -1;
        }
        applet->embedInto(wid);
    } else {
        std::cerr << "Usage: " << argv[0] << " [container-window-id]" << std::endl;
        return -2;
    }
    applet->show();
    return app.exec();
}

} // namespace

#include "applet.moc"
