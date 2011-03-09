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
#ifndef INTELLIHIDEBEHAVIOR_H
#define INTELLIHIDEBEHAVIOR_H

// Local

// Qt
#include <QObject>

struct _WnckWindow;

class MouseArea;
class Unity2dPanel;

/**
 * This class implements the Intellihide behavior of the launcher
 */
class IntelliHideBehavior : public QObject
{
Q_OBJECT
public:
    IntelliHideBehavior(Unity2dPanel* panel);
    ~IntelliHideBehavior();

protected:
    bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void updateVisibility();
    void updateActiveWindowConnections();
    void forceVisiblePanel();

private:
    Q_DISABLE_COPY(IntelliHideBehavior);

    enum PanelVisibility {
        VisiblePanel,
        HiddenPanel,
        ForceVisiblePanel
    };

    Unity2dPanel* m_panel;

    // A 1px invisible area used to force the panel to be visible if the user
    // hits the edge of the screen with the mouse cursor
    MouseArea* m_mouseArea;

    struct _WnckWindow* m_activeWindow;
    PanelVisibility m_visibility;

    void slidePanel();
    void updateFromPanelGeometry();
    void disconnectFromGSignals();
};

#endif /* INTELLIHIDEBEHAVIOR_H */
