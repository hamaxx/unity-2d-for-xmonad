#include "plugin.h"
#include "homebuttonapplet.h"

#include <QtPlugin>

QString HomeButtonPlugin::getAppletName() const
{
    return QString("homebutton");
}

QWidget* HomeButtonPlugin::getApplet() const
{
    return new HomeButtonApplet();
}

Q_EXPORT_PLUGIN2(panelplugin-homebutton, HomeButtonPlugin)

#include "plugin.moc"
