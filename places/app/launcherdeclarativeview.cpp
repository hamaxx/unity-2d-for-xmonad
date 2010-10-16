#include "launcherdeclarativeview.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QCloseEvent>
#include <QDeclarativeContext>

LauncherDeclarativeView::LauncherDeclarativeView() :
    QDeclarativeView()
{
}

void
LauncherDeclarativeView::fitToAvailableSpace(int screen)
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
LauncherDeclarativeView::closeEvent(QCloseEvent* event)
{
    event->ignore();
}

void
LauncherDeclarativeView::activate()
{
    rootContext()->setContextProperty("dashActive", QVariant(true));
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, false);
    raise();
    activateWindow();
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
}

void
LauncherDeclarativeView::deactivate()
{
    rootContext()->setContextProperty("dashActive", QVariant(false));
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, false);
    lower();
    clearFocus();
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);
}
