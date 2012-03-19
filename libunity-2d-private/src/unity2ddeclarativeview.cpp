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

#include "unity2ddeclarativeview.h"

#include <debug_p.h>
#include <config.h>

#include "screeninfo.h"

#include <QApplication>
#include <QDebug>
#include <QDeclarativeEngine>
#include <QDeclarativeItem>
#include <QGLWidget>
#include <QVariant>
#include <QX11Info>
#include <QFileInfo>
#include <QShowEvent>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QGraphicsView(parent),
    m_screenInfo(NULL),
    m_useOpenGL(false),
    m_transparentBackground(false),
    m_rootItem(NULL)
{
    setScene(&m_scene);

    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(NoFrame);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    viewport()->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    scene()->setStickyFocus(true);

    if (!QFileInfo(UNITY_2D_SCHEMA_FILE).exists()) {
        m_useOpenGL = false;
    } else {
        m_useOpenGL = unity2dConfiguration().property("useOpengl").toBool();
    }

    setupViewport();
}

Unity2DDeclarativeView::~Unity2DDeclarativeView()
{
}

QDeclarativeEngine* Unity2DDeclarativeView::engine()
{
    static QDeclarativeEngine* engine = new QDeclarativeEngine();
    return engine;
}

QDeclarativeContext* Unity2DDeclarativeView::rootContext() const
{
    return engine()->rootContext();
}

QDeclarativeItem* Unity2DDeclarativeView::rootObject() const
{
    return m_rootItem;
}

void Unity2DDeclarativeView::forceActivateWindow()
{
    forceActivateWindow(effectiveWinId(), this);
}

void Unity2DDeclarativeView::setSource(const QUrl &source, const QMap<const char*, QVariant> &rootObjectProperties)
{
    QDeclarativeComponent* component = new QDeclarativeComponent(engine(), source, this);
    QObject *instance = component->beginCreate(rootContext());
    if (component->isError()) {
        qDebug() << component->errors();
    }
    QMap<const char*, QVariant>::const_iterator it = rootObjectProperties.begin();
    QMap<const char*, QVariant>::const_iterator itEnd = rootObjectProperties.end();
    for ( ; it != itEnd; ++it) {
        instance->setProperty(it.key(), it.value());
    }
    component->completeCreate();
    m_rootItem = qobject_cast<QDeclarativeItem *>(instance);
    connect(m_rootItem, SIGNAL(widthChanged()), SLOT(resizeToRootObject()));
    connect(m_rootItem, SIGNAL(heightChanged()), SLOT(resizeToRootObject()));
    resizeToRootObject();
    m_scene.addItem(m_rootItem);
    m_source = source;
}

void Unity2DDeclarativeView::resizeToRootObject()
{
    QSize size(m_rootItem->width(), m_rootItem->height());
    resize(size);
    setSceneRect(QRectF(0, 0, size.width(), size.height()));
    Q_EMIT sceneResized(size);
}

bool Unity2DDeclarativeView::useOpenGL() const
{
    return m_useOpenGL;
}

void Unity2DDeclarativeView::setUseOpenGL(bool useOpenGL)
{
    if (useOpenGL == m_useOpenGL) {
        return;
    }

    m_useOpenGL = useOpenGL;
    setupViewport();

    Q_EMIT useOpenGLChanged(useOpenGL);
}

bool Unity2DDeclarativeView::transparentBackground() const
{
    return m_transparentBackground;
}

QUrl Unity2DDeclarativeView::source() const
{
    return m_source;
}

void Unity2DDeclarativeView::setTransparentBackground(bool transparentBackground)
{
    if (transparentBackground == m_transparentBackground) {
        return;
    }

    m_transparentBackground = transparentBackground;
    setupViewport();

    Q_EMIT transparentBackgroundChanged(transparentBackground);
}

QPoint Unity2DDeclarativeView::globalPosition() const
{
    // FIXME This used to be mapToGlobal(QPoint(0,0)) that is the correct
    // thing for all kind of widgets, but seems to fail sometimes if we
    // call it just after a moveEvent, which is bad
    // Since all our Unity2DDeclarativeView are toplevel windows we
    // are workarounding it by just returning pos()
    return pos();
}

void Unity2DDeclarativeView::setupViewport()
{
    if (m_useOpenGL) {
        QGLFormat format = QGLFormat::defaultFormat();
        format.setSampleBuffers(false);
        /* Synchronise rendering with vblank */
        format.setSwapInterval(1);

        QGLWidget *glWidget = new QGLWidget(format);
        /* TODO: possibly faster, to be tested, only works with non transparent windows */
        //glWidget->setAutoFillBackground(false);

        if (m_transparentBackground) {
            glWidget->setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_TranslucentBackground, true);
            /* automatically set by setting WA_TranslucentBackground */
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        } else {
            glWidget->setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        }

        setViewport(glWidget);
        /* According to Qt's documentation: "This is the preferred update mode
           for viewports that do not support partial updates, such as QGLWidget [...]"
        */
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    } else {
        setViewport(0);
        /* This is the default update mode */
        setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

        if (m_transparentBackground) {
            viewport()->setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_TranslucentBackground, true);
            /* automatically set by setting WA_TranslucentBackground */
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, false);
        } else {
            viewport()->setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_OpaquePaintEvent, true);
        }
    }
}

void Unity2DDeclarativeView::moveEvent(QMoveEvent* event)
{
    Q_EMIT globalPositionChanged(globalPosition());
}

void Unity2DDeclarativeView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    Q_EMIT visibleChanged(true);
}

void Unity2DDeclarativeView::hideEvent(QHideEvent* event)
{
    QGraphicsView::hideEvent(event);
    Q_EMIT visibleChanged(false);
}

void Unity2DDeclarativeView::keyPressEvent(QKeyEvent* event)
{
    QApplication::sendEvent(scene(), event);
}

void Unity2DDeclarativeView::keyReleaseEvent(QKeyEvent* event)
{
    QApplication::sendEvent(scene(), event);
}

void Unity2DDeclarativeView::forceActivateWindow(WId window, QWidget *w)
{
    /* Workaround focus stealing prevention implemented by some window
       managers such as Compiz. This is the exact same code you will find in
       libwnck::wnck_window_activate().

       ref.: http://permalink.gmane.org/gmane.comp.lib.qt.general/4733
    */
    Display* display = QX11Info::display();
    Atom net_wm_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW",
                                            False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.send_event = True;
    xev.xclient.display = display;
    xev.xclient.window = window;
    xev.xclient.message_type = net_wm_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(display, QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    /* Ensure focus is actually switched to active window */
    XSetInputFocus(display, window, RevertToParent, CurrentTime);
    XFlush(display);

    /* Use Qt's setFocus mechanism as a safety guard in case the above failed */
    if (w != NULL)
        w->setFocus();
}

ScreenInfo*
Unity2DDeclarativeView::screen() const
{
    return m_screenInfo;
}

#include <unity2ddeclarativeview.moc>
