#include "plugin.h"
#include "appnameapplet.h"

#include <QtPlugin>

QString AppNamePlugin::getAppletName() const
{
    return QString("appname");
}

QWidget* AppNamePlugin::getApplet() const
{
    return new Unity2d::AppNameApplet();
}

Q_EXPORT_PLUGIN2(panelplugin-appname, AppNamePlugin)

#include "plugin.moc"
