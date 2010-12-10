#include "dashdeclarativeview.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QCloseEvent>
#include <QDeclarativeContext>
#include <QX11Info>
#include <QGraphicsObject>

#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

DashDeclarativeView::DashDeclarativeView() :
    QDeclarativeView(), m_active(false)
{
    QDesktopWidget* desktop = QApplication::desktop();
    QObject::connect(desktop, SIGNAL(resized(int)), this, SIGNAL(screenGeometryChanged()));
    QObject::connect(desktop, SIGNAL(workAreaResized(int)), this, SIGNAL(availableGeometryChanged()));
}

void
DashDeclarativeView::fitToAvailableSpace(int screen)
{
    QDesktopWidget *desktop = QApplication::desktop();    
    int current_screen = desktop->screenNumber(this);

    if(screen == current_screen)
    {
        QRect geometry = desktop->availableGeometry(this);
        setGeometry(geometry);
        setFixedSize(geometry.size());
    }
}

void
DashDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    QDeclarativeView::focusOutEvent(event);
    setActive(false);
}

void
DashDeclarativeView::setActive(bool active)
{
    m_active = active;

    if(active)
    {
        emit activeChanged(active);
        show();
        raise();
        activateWindow();
        forceActivateWindow();
    }
    else
    {
        hide();
        emit activeChanged(active);
    }
}

bool
DashDeclarativeView::active() const
{
    return m_active;
}

void
DashDeclarativeView::forceActivateWindow()
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

void
DashDeclarativeView::activatePlaceEntry(const QString& file, const QString& entry)
{
    /* FIXME: this is a quick and dirty implementation.
       We need a cleaner way to access the place object and activate it. */
    QGraphicsObject* dash = rootObject();
    QString place_id = file.mid(file.lastIndexOf("/") + 1).replace(".", "_");
    QList<QObject*> matches = dash->findChildren<QObject*>(place_id);
    if (matches.size() > 0)
    {
        QObject* place = matches.at(0);
        setActive(true);
        QMetaObject::invokeMethod(dash, "activatePlace", Qt::AutoConnection,
                                  Q_ARG(QVariant, QVariant::fromValue(place)),
                                  Q_ARG(QVariant, QVariant::fromValue(0)));
    }
    else
    {
        qWarning() << "No matching place for" << file;
    }
}

void
DashDeclarativeView::activateHome()
{
    QGraphicsObject* dash = rootObject();
    setActive(true);
    QMetaObject::invokeMethod(dash, "activateHome", Qt::AutoConnection);
}

const QRect
DashDeclarativeView::screenGeometry() const
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->screenGeometry(this);
}

const QRect
DashDeclarativeView::availableGeometry() const
{
    QDesktopWidget* desktop = QApplication::desktop();
    return desktop->availableGeometry(this);
}

