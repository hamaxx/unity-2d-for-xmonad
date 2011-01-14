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

// Local
#include <launcherview.h>

// Qt
#include <QtTestGui>

class LauncherViewTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCreate()
    {
        LauncherView view;
    }
};

QTEST_MAIN(LauncherViewTest)

#include "launcherviewtest.moc"

