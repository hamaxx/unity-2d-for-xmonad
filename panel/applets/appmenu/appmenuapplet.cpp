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
#include "appmenuapplet.h"

// Local
#include "menubarwidget.h"

// Qt
#include <QHBoxLayout>

AppMenuApplet::AppMenuApplet()
: m_menuBarWidget(new MenuBarWidget)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_menuBarWidget);
}

#include "appmenuapplet.moc"
