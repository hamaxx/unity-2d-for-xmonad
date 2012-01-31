extern "C" {
#include <libwnck/libwnck.h>
}

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

#include "config.h"
#include "launcherclient.h"
#include "desktopinfo.h"
#include "workspacesinfo.h"

#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>

DesktopInfo::DesktopInfo(QObject *parent) :
    QObject(parent),
    m_activeWindow(0)
{
    WnckScreen *screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed",
                     G_CALLBACK(DesktopInfo::onActiveWindowChanged), NULL);

    updateActiveWindow(screen);
}


DesktopInfo* DesktopInfo::instance()
{
    static DesktopInfo* singleton = new DesktopInfo();
    return singleton;
}

void DesktopInfo::onActiveWindowChanged(WnckScreen *screen,
                                       WnckWindow *previously_active_window,
                                       gpointer    user_data)
{
    Q_UNUSED(previously_active_window);
    Q_UNUSED(user_data);

    DesktopInfo::instance()->updateActiveWindow(screen);
}

void DesktopInfo::updateActiveWindow(WnckScreen *screen)
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

bool DesktopInfo::isCompositingManagerRunning() const
{
    return QX11Info::isCompositingManagerRunning();
}

#include "desktopinfo.moc"
