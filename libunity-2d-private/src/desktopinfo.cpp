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
    QObject(parent)
{
    QDesktopWidget *dw = qApp->desktop();
    connect(dw, SIGNAL(screenCountChanged(int)), this, SIGNAL(totalWidthChanged()));
}


DesktopInfo* DesktopInfo::instance()
{
    static DesktopInfo* singleton = new DesktopInfo();
    return singleton;
}

bool DesktopInfo::isCompositingManagerRunning() const
{
    return QX11Info::isCompositingManagerRunning();
}

int DesktopInfo::totalWidth() const
{
    QDesktopWidget *dw = qApp->desktop();
    int tw = 0;
    for (int i = 0; i < dw->screenCount(); ++i) {
        tw += dw->screenGeometry(i).width();
    }
    return tw;
}

#include "desktopinfo.moc"
