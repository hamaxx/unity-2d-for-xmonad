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
#ifndef LEGACYTRAYAPPLET_H
#define LEGACYTRAYAPPLET_H

// Local
#include <applet.h>

// Qt

class LegacyTrayApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    LegacyTrayApplet();

private:
    Q_DISABLE_COPY(LegacyTrayApplet)
};

#endif /* LEGACYTRAYAPPLET_H */
