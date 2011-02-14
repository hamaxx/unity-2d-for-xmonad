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
#ifndef INTELLIHIDECONTROLLER_H
#define INTELLIHIDECONTROLLER_H

// Local

// Qt
#include <QObject>

struct _WnckWindow;

class Unity2dPanel;

/**
 * This class implements the Intellihide behavior of the launcher
 */
class IntellihideController : public QObject
{
Q_OBJECT
public:
    IntellihideController(Unity2dPanel* panel);
    ~IntellihideController();

private Q_SLOTS:
    void updateVisibility();
    void updateActiveWindowConnections();

private:
    Q_DISABLE_COPY(IntellihideController);
    Unity2dPanel* m_panel;

    struct _WnckWindow* m_activeWindow;
};

#endif /* INTELLIHIDECONTROLLER_H */
