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
#include <panel.h>

// Qt
#include <QApplication>
#include <QPainter>

// System
#include <iostream>

namespace Unity2d
{

struct AppletPrivate
{
};

Applet::Applet()
: QWidget()
, d(new AppletPrivate)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

Applet::~Applet()
{
    delete d;
}

} // namespace

#include "applet.moc"
