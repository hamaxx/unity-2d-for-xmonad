/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef EDGEMOUSEAREA_H
#define EDGEMOUSEAREA_H

// Local

// libunity-2d
#include <mousearea.h>

// Qt

/**
 * A mouse area which stays on the left edge of the screen
 */
class EdgeMouseArea : public MouseArea
{
Q_OBJECT
public:
    EdgeMouseArea(QObject* parent = 0);

    void setScreen(int);

private:
    int m_screen;

    void updateGeometryFromScreen();
};

#endif /* EDGEMOUSEAREA_H */
