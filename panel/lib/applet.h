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
#ifndef APPLET_H
#define APPLET_H

// Local

// Qt
#include <QWidget>

namespace UnityQt
{

struct AppletPrivate;
class Applet : public QWidget
{
Q_OBJECT
public:
    Applet();
    ~Applet();

private:
    Q_DISABLE_COPY(Applet)
    AppletPrivate* const d;
};

} // namespace

#endif /* APPLET_H */
