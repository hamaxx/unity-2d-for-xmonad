/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
 *
 * License: GPL v3
 */
#ifndef INTELLIHIDEBEHAVIOR_H
#define INTELLIHIDEBEHAVIOR_H

// Local
#include <abstractvisibilitybehavior.h>

// Qt
#include <QObject>

struct _WnckWindow;

class QTimer;

class QWidget;
class EdgeHitDetector;

/**
 * This class implements the Intellihide behavior of the launcher
 */
class IntelliHideBehavior : public AbstractVisibilityBehavior
{
    Q_OBJECT

    Q_PROPERTY(QWidget* panel READ panel WRITE setPanel NOTIFY panelChanged)

public:
    IntelliHideBehavior(QWidget* panel=0);
    ~IntelliHideBehavior();

    // setters
    void setPanel(QWidget *panel);

Q_SIGNALS:
    void panelChanged(QWidget *panel);

protected:
    bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void updateVisibility();
    void updateActiveWindowConnections();
    void showPanel();
    void hidePanel();

private:
    Q_DISABLE_COPY(IntelliHideBehavior);

    enum PanelVisibility {
        VisiblePanel,
        HiddenPanel
    };
    QTimer* m_updateVisibilityTimer;
    EdgeHitDetector* m_edgeHitDetector;

    struct _WnckWindow* m_activeWindow;

    void disconnectFromGSignals();

    bool isMouseForcingVisibility() const;

    void createEdgeHitDetector();
};

#endif /* INTELLIHIDEBEHAVIOR_H */
