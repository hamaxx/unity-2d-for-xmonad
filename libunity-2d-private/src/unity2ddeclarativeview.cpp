#include "unity2ddeclarativeview.h"
#include <QDebug>
#include <QGLWidget>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <debug_p.h>

Unity2DDeclarativeView::Unity2DDeclarativeView(QWidget *parent) :
    QDeclarativeView(parent), m_useOpenGL(false), m_transparentBackground(false), m_last_focused_window(None)
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
    // Save reference to window with current keyboard focus
    if( m_last_focused_window == None ){
        saveActiveWindow();
    }

    // Show this window by giving it keyboard focus
    forceActivateThisWindow(this->effectiveWinId());
}

void Unity2DDeclarativeView::forceDeactivateWindow()
{
    if( m_last_focused_window == None ){
        UQ_WARNING << "No previously focused window found, use mouse to select window.";
        //don't restore keyboard focus
        return;
    }

    // What if window closed while we were in launcher? Check if window
    // exists by seeing if it has attributes.
    int status;
    XWindowAttributes attributes;
    status = XGetWindowAttributes(QX11Info::display(), m_last_focused_window, &attributes);
    if ( status == BadWindow ){
        UQ_WARNING << "Previously focused window has gone, use mouse to select window.";
        return;
    }

    // Show this window by giving it keyboard focus
    forceActivateThisWindow(m_last_focused_window);

    m_last_focused_window = None;
}

void Unity2DDeclarativeView::forceActivateThisWindow(WId window)
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
}

/* Save WId of window with keyboard focus to m_last_focused_window */
void Unity2DDeclarativeView::saveActiveWindow()
{
    Display* display = QX11Info::display();
    WId active_window;
    int current_focus_state;

    XGetInputFocus(display, &active_window, &current_focus_state);
    if( active_window != this->effectiveWinId()){
        m_last_focused_window = active_window;
    }
}


#include <unity2ddeclarativeview.moc>
