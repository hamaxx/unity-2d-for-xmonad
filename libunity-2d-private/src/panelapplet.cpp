#include "panelapplet.h"

namespace Unity2d
{

class PanelAppletPrivate
{
    Unity2dPanel* m_panel;

    PanelApplet* q_ptr;
    Q_DECLARE_PUBLIC(PanelApplet)
};

PanelApplet::PanelApplet(Unity2dPanel* panel) :
    QWidget(),
    d_ptr(new PanelAppletPrivate)
{
    Q_D(PanelApplet);

    d->m_panel = panel;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

Unity2dPanel* PanelApplet::panel() const
{
    Q_D(const PanelApplet);
    return d->m_panel;
}

} // namespace Unity2d

#include "panelapplet.moc"
