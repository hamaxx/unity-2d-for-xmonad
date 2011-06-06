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
#ifndef EDGEHITDETECTOR_H
#define EDGEHITDETECTOR_H

// Local

// libunity-2d
#include <mousearea.h>

// Qt

class QTimer;

/**
 * A mouse area which stays on the left edge of the leftmost screen
 */
class EdgeHitDetector : public QObject
{
Q_OBJECT
public:
    EdgeHitDetector(QObject* parent = 0);

Q_SIGNALS:
    void edgeHit();

private Q_SLOTS:
    void updateGeometryFromScreen();

private:
    MouseArea* m_mouseArea;
    QTimer* m_updateTimer;
    QTimer* m_edgeHitTimer;
};

#endif /* EDGEHITDETECTOR_H */
