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

#include "desktopinfo.moc"
