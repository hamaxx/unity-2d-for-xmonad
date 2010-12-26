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
#ifndef PANEL_H
#define PANEL_H

// Local

// Qt
#include <QWidget>

namespace UnityQt
{

struct PanelPrivate;
class Panel : public QWidget
{
Q_OBJECT
public:
    enum Edge {
        LeftEdge,
        TopEdge
    };

    Panel(QWidget* parent = 0);
    ~Panel();

    void setEdge(Edge);
    Edge edge() const;

    void addWidget(QWidget*);

    void addSpacer();

protected:
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    Q_DISABLE_COPY(Panel)
    PanelPrivate* const d;

protected Q_SLOTS:
    void desktopResized(int screen);
};

} // namespace

#endif /* PANEL_H */
