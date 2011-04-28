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

class QTimer;

/**
 * A mouse area which stays on the left edge of the leftmost screen
 */
class EdgeMouseArea : public MouseArea
{
Q_OBJECT
public:
    EdgeMouseArea(QObject* parent = 0);

private Q_SLOTS:
    void updateGeometryFromScreen();

private:
    QTimer* m_updateTimer;
};

#endif /* EDGEMOUSEAREA_H */
