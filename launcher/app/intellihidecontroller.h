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

protected:
    bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void updateVisibility();
    void updateActiveWindowConnections();

private:
    Q_DISABLE_COPY(IntellihideController);

    enum PanelVisibility {
        VisiblePanel,
        HiddenPanel,
        ForceVisiblePanel
    };

    Unity2dPanel* m_panel;

    // A 1px invisible panel used to force the panel to be visible if the user
    // hits the edge of the screen with the mouse cursor
    Unity2dPanel* m_forceVisiblePanel;

    struct _WnckWindow* m_activeWindow;
    PanelVisibility m_visibility;

    void slidePanel();
};

#endif /* INTELLIHIDECONTROLLER_H */
