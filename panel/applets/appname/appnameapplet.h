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
#ifndef APPNAMEAPPLET_H
#define APPNAMEAPPLET_H

// Local
#include <applet.h>

// Qt

namespace UnityQt
{

struct AppNameAppletPrivate;
class AppNameApplet : public Applet
{
Q_OBJECT
public:
    AppNameApplet();
    ~AppNameApplet();

private Q_SLOTS:
    void updateLabel();
    void updateWindowHelper();
    void updateWidgets();

private:
    Q_DISABLE_COPY(AppNameApplet)
    AppNameAppletPrivate* const d;
};

} // namespace

#endif /* APPNAMEAPPLET_H */

