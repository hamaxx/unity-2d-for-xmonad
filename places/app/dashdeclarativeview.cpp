#include "dashdeclarativeview.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QCloseEvent>
#include <QDeclarativeContext>

DashDeclarativeView::DashDeclarativeView() :
    QDeclarativeView(), m_active(false)
{
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
DashDeclarativeView::closeEvent(QCloseEvent* event)
{
    event->ignore();
}

void
DashDeclarativeView::setActive(bool active)
{
    m_active = active;

    if(active)
    {
        grabKeyboard();
        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, false);
        raise();
        activateWindow();
        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
    }
    else
    {
        releaseKeyboard();
        setAttribute(Qt::WA_X11NetWmWindowTypeDock, false);
        lower();
        clearFocus();
        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);
    }

    emit activeChanged(active);
}

bool
DashDeclarativeView::active() const
{
    return m_active;
}
