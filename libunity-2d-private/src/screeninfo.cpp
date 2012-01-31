#include "config.h"
#include "launcherclient.h"
#include "screeninfo.h"

#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>

ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent)
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
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

#include "screeninfo.moc"
