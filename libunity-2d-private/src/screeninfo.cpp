#include "config.h"
#include "launcherclient.h"
#include "screeninfo.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QVariant>
#include <QX11Info>

ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent),
    m_screen(QX11Info::appScreen()),
    m_widget(NULL),
    m_corner(InvalidCorner)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}

ScreenInfo::ScreenInfo(int screen, QObject *parent) :
    QObject(parent),
    m_screen(screen),
    m_widget(NULL),
    m_corner(InvalidCorner)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}

ScreenInfo::ScreenInfo(QWidget *widget, QObject *parent) :
    QObject(parent),
    m_screen(QApplication::desktop()->screenNumber(widget)),
    m_widget(widget),
    m_corner(InvalidCorner)
{
    m_widget->installEventFilter(this);
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}

ScreenInfo::ScreenInfo(Corner corner, QObject *parent) :
    QObject(parent),
    m_screen(cornerScreen(corner)),
    m_widget(NULL),
    m_corner(corner)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}

ScreenInfo::~ScreenInfo()
{
    if (m_widget) {
        m_widget->removeEventFilter(this);
    }
}

QRect ScreenInfo::availableGeometry() const
{
    return QApplication::desktop()->availableGeometry(m_screen);
}

QRect ScreenInfo::panelsFreeGeometry() const
{
    /* We cannot just return the system's availableGeometry(), because that
     * doesn't consider the Launcher, if it's set to auto-hide. */
    QRect screenRect = QApplication::desktop()->screenGeometry(m_screen);
    QRect availableRect = QApplication::desktop()->availableGeometry(m_screen);

    const bool accountForLauncher = !launcher2dConfiguration().property("onlyOneLauncher").toBool() || m_screen == 0;

    QRect availableGeometry(
        screenRect.left() + (accountForLauncher ? LauncherClient::MaximumWidth : 0),
        availableRect.top(),
        screenRect.width() - (accountForLauncher ? LauncherClient::MaximumWidth : 0),
        availableRect.height()
        );
    if (QApplication::isRightToLeft()) {
        availableGeometry.moveLeft(screenRect.left());
    }
    return availableGeometry;
}

QRect ScreenInfo::geometry() const
{
    return QApplication::desktop()->screenGeometry(m_screen);
}

void ScreenInfo::updateGeometry(int screen)
{
    if (m_corner != InvalidCorner) {
        int screenCorner = cornerScreen(m_corner);
        if (m_screen != screenCorner) {
            setScreen(screenCorner);
            return;
        }
    }
    if (screen == m_screen) {
        Q_EMIT geometryChanged(geometry());
    }
}

void ScreenInfo::updateAvailableGeometry(int screen)
{
    if (screen == m_screen) {
        Q_EMIT availableGeometryChanged(availableGeometry());
        Q_EMIT panelsFreeGeometryChanged(panelsFreeGeometry());
    }
}

void ScreenInfo::updateScreen()
{
    int screen;
    if (m_corner != InvalidCorner) {
        screen = cornerScreen(m_corner);
        setScreen(screen);
    } else if (m_widget) {
        screen = QApplication::desktop()->screenNumber(m_widget);
        setScreen(screen);
    }
}

int
ScreenInfo::cornerScreen(Corner corner) const
{
    QDesktopWidget* desktop = QApplication::desktop();
    switch(corner) {
        case TopLeft:
            return desktop->screenNumber(QPoint());
        case TopRight:
            return desktop->screenNumber(QPoint(desktop->width(), 0));
        case BottomLeft:
            return desktop->screenNumber(QPoint(0, desktop->height()));
        case BottomRight:
            return desktop->screenNumber(QPoint(desktop->width(), desktop->height()));
        default:
            return desktop->screenNumber(QPoint());
    }
}

bool
ScreenInfo::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::Move || event->type() == QEvent::Show) {
        updateScreen();
    }
    return QObject::eventFilter(object, event);
}

int
ScreenInfo::screen() const
{
    return m_screen;
}

void
ScreenInfo::setScreen(int screen)
{
    if (m_screen != screen) {
        m_screen = screen;
        Q_EMIT screenChanged(m_screen);
        Q_EMIT geometryChanged(geometry());
        Q_EMIT availableGeometryChanged(availableGeometry());
        Q_EMIT panelsFreeGeometryChanged(panelsFreeGeometry());
    }
}

QWidget*
ScreenInfo::widget() const
{
    return m_widget;
}

void
ScreenInfo::setWidget(QWidget *widget)
{
    if (m_widget != widget) {
        m_widget->removeEventFilter(this);
        m_widget = widget;
        if (m_widget) {
            m_widget->installEventFilter(this);
        }
        Q_EMIT widgetChanged(m_widget);
        updateScreen();
    }
}

ScreenInfo::Corner
ScreenInfo::corner() const
{
    return m_corner;
}

void
ScreenInfo::setCorner(Corner corner)
{
    if (m_corner != corner) {
        m_corner = corner;
        Q_EMIT cornerChanged(corner);
        updateScreen();
    }
}

#include "screeninfo.moc"
