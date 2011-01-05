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
#include <homebuttonapplet.h>

// Qt
#include <QtTestGui>

using namespace UnityQt;

class HomeButtonAppletTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCreateButton()
    {
        HomeButtonApplet applet;
    }
};

QTEST_MAIN(HomeButtonAppletTest)

#include "homebuttonapplettest.moc"
