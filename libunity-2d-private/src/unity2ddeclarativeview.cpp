#include "unity2ddeclarativeview.h"
#include <QGLWidget>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QDeclarativeView(parent), m_useOpenGL(false), m_transparentBackground(false)
{
    setupViewport();
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

void Unity2DDeclarativeView::setTransparentBackground(bool transparentBackground)
{
    if (transparentBackground == m_transparentBackground) {
        return;
    }

    m_transparentBackground = transparentBackground;
    setupViewport();

    Q_EMIT transparentBackgroundChanged(transparentBackground);
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

void Unity2DDeclarativeView::forceActivateWindow()
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
    xev.xclient.window = this->effectiveWinId();
    xev.xclient.message_type = net_wm_active_window;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2;
    xev.xclient.data.l[1] = CurrentTime;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(display, QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

#include <unity2ddeclarativeview.moc>
