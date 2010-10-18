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
// Self
#include "legacytrayapplet.h"

// Local
#include <debug_p.h>

// Qt
#include <QHBoxLayout>

LegacyTrayApplet::LegacyTrayApplet()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
}

#include "legacytrayapplet.moc"
