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

class QEvent;

namespace Unity2d
{

struct AppNameAppletPrivate;
class AppNameApplet : public Applet
{
Q_OBJECT
public:
    AppNameApplet();
    ~AppNameApplet();

protected:
    bool event(QEvent*); // reimp
    bool eventFilter(QObject*, QEvent*); // reimp

private Q_SLOTS:
    void updateWidgets();

private:
    Q_DISABLE_COPY(AppNameApplet)
    AppNameAppletPrivate* const d;
};

} // namespace

#endif /* APPNAMEAPPLET_H */

