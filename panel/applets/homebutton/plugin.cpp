#include "plugin.h"
#include "homebutton.h"

#include <QtPlugin>

QString HomeButtonPlugin::getAppletName() const
{
    return QString("homebutton");
}

QWidget* HomeButtonPlugin::getApplet() const
{
    return new HomeButton();
}

Q_EXPORT_PLUGIN2(panelplugin-homebutton, HomeButtonPlugin)

#include "plugin.moc"
