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
class IndicatorsManager;

// Qt
#include <QWidget>
#include <QEvent>

struct Unity2dPanelPrivate;
class Unity2dPanel : public QWidget
{
    Q_OBJECT
    /**
     * The amount of pixels the panel is moved from its edge. Useful for
     * animations.
     */
    Q_PROPERTY(int delta READ delta WRITE setDelta)
    /**
     * Whether the delta property is being set by an external client. Setting
     * this property to true stops any animation of the delta property triggered
       by slideIn() or slideOut().
     */
    Q_PROPERTY(int manualSliding READ manualSliding WRITE setManualSliding NOTIFY manualSlidingChanged)
    /** The time the slide-in takes. */
    Q_PROPERTY(int slideDuration READ slideDuration WRITE setSlideDuration NOTIFY slideDurationChanged)

    Q_PROPERTY(bool useStrut READ useStrut WRITE setUseStrut NOTIFY useStrutChanged)

    /** The physical screen this panel is on. */
    Q_PROPERTY(int screen READ screen WRITE setScreen NOTIFY screenChanged)
    /** The thickness of the panel in px. */
    Q_PROPERTY(int panelSize READ panelSize WRITE setPanelSize NOTIFY panelSizeChanged)
    /**
     * The intervall between the fallback geometry updates (the last resort if 
     * there is no notification about a configuration change)
     */
    Q_PROPERTY(int fallbackRedrawIntervall READ fallbackRedrawIntervall WRITE setFallbackRedrawIntervall NOTIFY fallbackRedrawIntervallChanged)

public:
    enum Edge {
        LeftEdge,
	RightEdge,
	TopEdge,
	BottomEdge
    };

    static const QEvent::Type SHOW_FIRST_MENU_EVENT = QEvent::User;

    Unity2dPanel(bool requiresTransparency = false, QWidget* parent = 0);
    ~Unity2dPanel();

    void setEdge(Edge);
    Edge edge() const;

    void addWidget(QWidget*);

    void addSpacer();

    IndicatorsManager* indicatorsManager() const;

    /**
     * Whether the panel should reserve space on the edge, preventing maximized
     * windows to overlap it.
     */
    bool useStrut() const;
    void setUseStrut(bool);

    int delta() const;
    void setDelta(int);

    int panelSize() const;
    void setPanelSize(int);

    bool manualSliding() const;
    void setManualSliding(bool);

    int screen() const;
    void setScreen(int);

    int fallbackRedrawIntervall() const;
    void setFallbackRedrawIntervall(int);

    int slideDuration() const;
    void setSlideDuration(int);

public Q_SLOTS:
    void slideIn();
    void slideOut();

Q_SIGNALS:
    void manualSlidingChanged(bool);
    void useStrutChanged(bool);
    void screenChanged(int);
    void fallbackRedrawIntervallChanged(int);
    void slideDurationChanged(int);
    void panelSizeChanged(int);

protected:
    virtual void showEvent(QShowEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
    void slotWorkAreaResized(int screen);
    void slotScreenCountChanged(int screenno);
    /** If everything else fails. Invoked using a timer. This is a dirty hack. */
    void slotFallbackGeometryUpdate();
    
private:
    Q_DISABLE_COPY(Unity2dPanel)
    Unity2dPanelPrivate* const d;
};

#endif /* UNITY2DPANEL_H */
