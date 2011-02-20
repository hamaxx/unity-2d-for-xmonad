/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UNITY2DPANEL_H
#define UNITY2DPANEL_H

// Local

// Qt
#include <QWidget>

struct Unity2dPanelPrivate;
class Unity2dPanel : public QWidget
{
    Q_OBJECT
    /**
     * The amount of pixels the panel is moved from its edge. Useful for
     * animations.
     */
    Q_PROPERTY(int delta READ delta WRITE setDelta)
public:
    enum Edge {
        LeftEdge,
        TopEdge
    };

    Unity2dPanel(QWidget* parent = 0);
    ~Unity2dPanel();

    void setEdge(Edge);
    Edge edge() const;

    void addWidget(QWidget*);

    void addSpacer();

    /**
     * Whether the panel should reserve space on the edge, preventing maximized
     * windows to overlap it.
     */
    bool useStrut() const;
    void setUseStrut(bool);

    int delta() const;
    void setDelta(int);

    int panelSize() const;

public Q_SLOTS:
    void slideIn();
    void slideOut();

protected:
    virtual void showEvent(QShowEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
    void slotWorkAreaResized(int screen);

private:
    Q_DISABLE_COPY(Unity2dPanel)
    Unity2dPanelPrivate* const d;
};

#endif /* UNITY2DPANEL_H */
