/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PANELSTYLE_H
#define PANELSTYLE_H

// Local

// Qt
#include <QObject>

class QColor;
class QFont;
class QPixmap;

struct _GtkStyleContext;

class PanelStylePrivate;
/**
 * Provides easy access to panel colors
 */
class PanelStyle : public QObject
{
    Q_OBJECT
public:
    PanelStyle(QObject* parent = 0);
    ~PanelStyle();

    enum WindowButtonType {
        CloseWindowButton,
        MinimizeWindowButton,
        UnmaximizeWindowButton
    };

    enum WindowButtonState {
        NormalState,
        PrelightState,
        PressedState
    };

    static PanelStyle* instance();

    QColor textColor() const;
    QColor backgroundTopColor() const;
    QColor backgroundBottomColor() const;
    QColor textShadowColor() const;
    QColor lineColor() const;

    QFont font() const;

    struct _GtkStyleContext* styleContext() const;

    QPixmap windowButtonPixmap(WindowButtonType, WindowButtonState);

private:
    friend class PanelStylePrivate;
    // Use a pimpl to avoid the need for gtk includes here
    PanelStylePrivate* const d;
};

#endif /* PANELSTYLE_H */
