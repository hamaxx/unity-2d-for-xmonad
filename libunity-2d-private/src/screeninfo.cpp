#include "config.h"
#include "launcherclient.h"
#include "screeninfo.h"

#include <QApplication>
#include <QDesktopWidget>

ScreenInfo::ScreenInfo(QWidget *widget, QObject *parent) :
    QObject(parent),
    m_screen(QApplication::desktop()->screenNumber(widget)),
    m_widget(widget)
{
    m_widget->installEventFilter(this);
    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
}

ScreenInfo::~ScreenInfo()
{
    m_widget->removeEventFilter(this);
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
    return QApplication::desktop()->screenGeometry(m_screen);
}

void ScreenInfo::updateGeometry(int screen)
{
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
    int screen = QApplication::desktop()->screenNumber(m_widget);
    if (screen != m_screen) {
        m_screen = screen;
        Q_EMIT geometryChanged(geometry());
        Q_EMIT availableGeometryChanged(availableGeometry());
        Q_EMIT panelsFreeGeometryChanged(panelsFreeGeometry());
    }
}

bool
ScreenInfo::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::Move) {
        updateScreen();
    }
    return QObject::eventFilter(object, event);
}

#include "screeninfo.moc"
