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
#ifndef APPMENUAPPLET_H
#define APPMENUAPPLET_H

// Local
#include <applet.h>

// Qt

class MenuBarWidget;

class AppMenuApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    AppMenuApplet();

private:
    MenuBarWidget* m_menuBarWidget;
};

#endif /* APPMENUAPPLET_H */
