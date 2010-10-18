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

namespace SystemTray
{
class FdoSelectionManager;
class Task;
}

class LegacyTrayApplet : public UnityQt::Applet
{
Q_OBJECT
public:
    LegacyTrayApplet();
    virtual ~LegacyTrayApplet();

private Q_SLOTS:
    void slotTaskCreated(SystemTray::Task*);

private:
    Q_DISABLE_COPY(LegacyTrayApplet)
    
    SystemTray::FdoSelectionManager* m_selectionManager;
};

#endif /* LEGACYTRAYAPPLET_H */
