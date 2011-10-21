extern "C" {
#include <libwnck/libwnck.h>
}

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

#include "launcherclient.h"
#include "screeninfo.h"
#include "workspacesinfo.h"

#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>

ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent),
    m_activeWindow(0)
{
    WnckScreen *screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed",
                     G_CALLBACK(ScreenInfo::onActiveWindowChanged), NULL);

    updateActiveWindow(screen);

    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}


ScreenInfo* ScreenInfo::instance()
{
    static ScreenInfo* singleton = new ScreenInfo();
    return singleton;
}

void ScreenInfo::onActiveWindowChanged(WnckScreen *screen,
                                       WnckWindow *previously_active_window,
                                       gpointer    user_data)
{
    Q_UNUSED(previously_active_window);
    Q_UNUSED(user_data);

    ScreenInfo::instance()->updateActiveWindow(screen);
}

void ScreenInfo::updateActiveWindow(WnckScreen *screen)
{
    unsigned int activeWindow = 0;
    WnckWindow *wnckActiveWindow = wnck_screen_get_active_window(screen);
    if (wnckActiveWindow != NULL) {
        activeWindow = wnck_window_get_xid(wnckActiveWindow);
    }

    if (activeWindow != m_activeWindow) {
        m_activeWindow = activeWindow;
        Q_EMIT activeWindowChanged(m_activeWindow);
    }
}

/* FIXME: This should be removed when we find a cleaner way to bypass the
   QML Image cache. See SpreadWindow.qml and WindowImageProvider::requestImage
   for details. */
QString ScreenInfo::currentTime()
{
    return QString::number(time(NULL));
}

QRect ScreenInfo::availableGeometry() const
{
    int screen = QX11Info::appScreen();
    return QApplication::desktop()->availableGeometry(screen);
}

QRect ScreenInfo::panelsFreeGeometry() const
{
    /* We cannot just return the system's availableGeometry(), because that
     * doesn't consider the Launcher, if it's set to auto-hide. */
    int screen = QX11Info::appScreen();
    QRect screenRect = QApplication::desktop()->screenGeometry(screen);
    QRect availableRect = QApplication::desktop()->availableGeometry(screen);

    QRect availableGeometry(
        LauncherClient::MaximumWidth,
        availableRect.top(),
        screenRect.width() - LauncherClient::MaximumWidth,
        availableRect.height()
        );
    if (QApplication::isRightToLeft()) {
        availableGeometry.moveLeft(0);
    }
    return availableGeometry;
}

QRect ScreenInfo::geometry() const
{
    return QApplication::desktop()->screenGeometry(QX11Info::appScreen());
}

void ScreenInfo::updateGeometry(int screen)
{
    if (screen == QX11Info::appScreen()) {
        Q_EMIT geometryChanged(geometry());
    }
}

void ScreenInfo::updateAvailableGeometry(int screen)
{
    if (screen == QX11Info::appScreen()) {
        Q_EMIT availableGeometryChanged(availableGeometry());
        Q_EMIT panelsFreeGeometryChanged(panelsFreeGeometry());
    }
}

bool ScreenInfo::isCompositingManagerRunning() const
{
    return QX11Info::isCompositingManagerRunning();
}

#include "screeninfo.moc"
