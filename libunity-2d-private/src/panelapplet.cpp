#include "panelapplet.h"

namespace Unity2d
{

PanelApplet::PanelApplet(QWidget* parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

} // namespace Unity2d

#include "panelapplet.moc"
