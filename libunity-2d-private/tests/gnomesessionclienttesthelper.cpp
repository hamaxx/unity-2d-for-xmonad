/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */

// Local
#include <gnomesessionclient.h>

// Qt
#include <QApplication>
#include <QPushButton>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    GnomeSessionClient client("/tmp/gnomesessionclient-test.desktop");
    client.connectToSessionManager();
    QPushButton button("GnomeSessionClient Test : click me to quit");
    QObject::connect(&button, SIGNAL(clicked()), &app, SLOT(quit()));
    button.show();
    return app.exec();
}
