/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#include "dashdeclarativeview.h"
#include "dashdbus.h"

// unity-2d
#include <launcherclient.h>
#include <screeninfo.h>

// Qt
#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QDeclarativeContext>
#include <QX11Info>
#include <QGraphicsObject>
#include <QtDBus/QDBusConnection>
#include <QTimer>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <config.h>

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

static const int DASH_DESKTOP_WIDTH = 989;
static const int DASH_DESKTOP_COLLAPSED_HEIGHT = 115;
static const int DASH_DESKTOP_EXPANDED_HEIGHT = 606;

DashDeclarativeView::DashDeclarativeView()
: Unity2DDeclarativeView()
, m_launcherClient(new LauncherClient(this))
, m_mode(DesktopMode)
, m_expanded(true)
, m_active(false)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setTransparentBackground(QX11Info::isCompositingManagerRunning());

    ScreenInfo* screen = ScreenInfo::instance();
    connect(screen, SIGNAL(geometryChanged(QRect)), SLOT(updateDashModeDependingOnScreenGeometry()));
    connect(screen, SIGNAL(availableGeometryChanged(QRect)), SLOT(updateSize()));

    updateSize();
}

static int getenvInt(const char* name, int defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    int value = stringValue.toInt(&ok);
    return ok ? value : defaultValue;
}

void
DashDeclarativeView::updateDashModeDependingOnScreenGeometry()
{
    QRect rect = ScreenInfo::instance()->geometry();
    static int minWidth = getenvInt("DASH_MIN_SCREEN_WIDTH", DASH_MIN_SCREEN_WIDTH);
    static int minHeight = getenvInt("DASH_MIN_SCREEN_HEIGHT", DASH_MIN_SCREEN_HEIGHT);
    if (rect.width() < minWidth && rect.height() < minHeight) {
        setDashMode(FullScreenMode);
    } else {
        setDashMode(DesktopMode);
    }
}

void
DashDeclarativeView::updateSize()
{
    if (m_mode == FullScreenMode) {
        fitToAvailableSpace();
    } else {
        resizeToDesktopModeSize();
    }
}

void
DashDeclarativeView::fitToAvailableSpace()
{
    QRect rect = ScreenInfo::instance()->availableGeometry();
    move(rect.topLeft());
    setFixedSize(rect.size());
}

void
DashDeclarativeView::resizeToDesktopModeSize()
{
    QRect rect = ScreenInfo::instance()->availableGeometry();
    int screenRight = rect.right();

    rect.setWidth(qMin(DASH_DESKTOP_WIDTH, rect.width()));
    rect.setHeight(qMin(m_expanded ? DASH_DESKTOP_EXPANDED_HEIGHT : DASH_DESKTOP_COLLAPSED_HEIGHT,
                        rect.height()));

    if (QApplication::isRightToLeft()) {
        rect.moveRight(screenRight);
    }

    move(rect.topLeft());
    setFixedSize(rect.size());
}

void
DashDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    QDeclarativeView::focusOutEvent(event);
    setActive(false);
}

void
DashDeclarativeView::setWMFlags()
{
    Display *display = QX11Info::display();
    Atom stateAtom = XInternAtom(display, "_NET_WM_STATE", False);
    Atom propAtom;

    propAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    XChangeProperty(display, effectiveWinId(), stateAtom,
                    XA_ATOM, 32, PropModeAppend, (unsigned char *) &propAtom, 1);

    propAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(display, effectiveWinId(), stateAtom,
                    XA_ATOM, 32, PropModeAppend, (unsigned char *) &propAtom, 1);
}

void
DashDeclarativeView::showEvent(QShowEvent *event)
{
    QDeclarativeView::showEvent(event);
    /* Note that this has to be called everytime the window is shown, as the WM
       will remove the flags when the window is hidden */
    setWMFlags();
}

void
DashDeclarativeView::setActive(bool value)
{
    if (value != m_active) {
        m_active = value;
        if (value) {
            updateDashModeDependingOnScreenGeometry();
            show();
            raise();
            // We need a delay, otherwise the window may not be visible when we try to activate it
            QTimer::singleShot(0, this, SLOT(forceActivateWindow()));
            m_launcherClient->beginForceVisible();
        } else {
            hide();
            m_launcherClient->endForceVisible();
        }
        activeChanged(m_active);
    }
}

bool
DashDeclarativeView::active() const
{
    return m_active;
}

void
DashDeclarativeView::setDashMode(DashDeclarativeView::DashMode mode)
{
    if (m_mode == mode) {
        return;
    }

    m_mode = mode;
    updateSize();
    dashModeChanged(m_mode);
}

DashDeclarativeView::DashMode
DashDeclarativeView::dashMode() const
{
    return m_mode;
}

void
DashDeclarativeView::setExpanded(bool value)
{
    if (m_expanded == value) {
        return;
    }

    m_expanded = value;
    updateSize();
    expandedChanged(m_expanded);
}

bool
DashDeclarativeView::expanded() const
{
    return m_expanded;
}

void
DashDeclarativeView::setActiveLens(const QString& activeLens)
{
    if (activeLens != m_activeLens) {
        m_activeLens = activeLens;
        Q_EMIT activeLensChanged(activeLens);
    }
}

const QString&
DashDeclarativeView::activeLens() const
{
    return m_activeLens;
}

void
DashDeclarativeView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            setActive(false);
            break;
        default:
            QDeclarativeView::keyPressEvent(event);
            break;
    }
}

void
DashDeclarativeView::resizeEvent(QResizeEvent* event)
{
    if (!QX11Info::isCompositingManagerRunning()) {
        updateMask();
    }
    QDeclarativeView::resizeEvent(event);
}

static QBitmap
createCornerMask()
{
    QPixmap pix(unity2dDirectory() + "/places/artwork/desktop_dash_background_no_transparency.png");
    return pix.createMaskFromColor(Qt::red, Qt::MaskOutColor);
}

void
DashDeclarativeView::updateMask()
{
    if (m_mode == FullScreenMode) {
        clearMask();
        return;
    }
    QBitmap bmp(size());
    {
        static QBitmap corner = createCornerMask();
        static QBitmap top = corner.copy(0, 0, corner.width(), 1);
        static QBitmap left = corner.copy(0, 0, 1, corner.height());

        const int cornerX = bmp.width() - corner.width();
        const int cornerY = bmp.height() - corner.height();

        QPainter painter(&bmp);
        painter.fillRect(bmp.rect(), Qt::color1);
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(Qt::color1);
        painter.setPen(Qt::color0);
        painter.drawPixmap(cornerX, cornerY, corner);

        painter.drawTiledPixmap(cornerX, 0, top.width(), cornerY, top);
        painter.drawTiledPixmap(0, cornerY, cornerX, left.height(), left);
    }
    setMask(bmp);
}
