/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#ifndef UNITY2DDECLARATIVEVIEW_H
#define UNITY2DDECLARATIVEVIEW_H

#include <QDeclarativeView>

class ScreenInfo;

class Unity2DDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool useOpenGL READ useOpenGL WRITE setUseOpenGL NOTIFY useOpenGLChanged)
    Q_PROPERTY(bool transparentBackground READ transparentBackground WRITE setTransparentBackground NOTIFY transparentBackgroundChanged)
    Q_PROPERTY(QPoint globalPosition READ globalPosition NOTIFY globalPositionChanged)
    Q_PROPERTY(ScreenInfo* screen READ screen NOTIFY screenChanged)
    Q_PROPERTY(bool visible READ isVisible NOTIFY visibleChanged)

public:
    Unity2DDeclarativeView(QWidget *parent = 0);
    ~Unity2DDeclarativeView();

    // getters
    bool useOpenGL() const;
    bool transparentBackground() const;
    QPoint globalPosition() const;
    ScreenInfo* screen() const;

    // setters
    void setUseOpenGL(bool);
    void setTransparentBackground(bool);

Q_SIGNALS:
    void useOpenGLChanged(bool);
    void transparentBackgroundChanged(bool);
    void globalPositionChanged(QPoint);
    void screenChanged(ScreenInfo*);
    void visibleChanged(bool);
    void activeWorkspaceChanged();

protected:
    void setupViewport();
    virtual void moveEvent(QMoveEvent* event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent* event);

    ScreenInfo* m_screenInfo;

protected Q_SLOTS:
    void forceActivateWindow();
    void forceDeactivateWindow();

private Q_SLOTS:
    void onActiveWorkspaceChanged();

private:
    void saveActiveWindow();
    void forceActivateThisWindow(WId);

    bool m_useOpenGL;
    bool m_transparentBackground;
    WId m_last_focused_window;
};

Q_DECLARE_METATYPE(Unity2DDeclarativeView*)

#endif // UNITY2DDECLARATIVEVIEW_H
