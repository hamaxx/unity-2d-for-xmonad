/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Self
#include "panel.h"

// Local
#include "applet.h"

// Qt
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QHBoxLayout>
#include <QX11Info>

// X
#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace Unity2d
{

struct PanelPrivate
{
    Panel* q;
    Panel::Edge m_edge;
    QHBoxLayout* m_layout;

    void updateStrut()
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect screen = desktop->screenGeometry(q);
        const QRect available = desktop->availableGeometry(q);
        QRect rect;

        Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);

        ulong struts[12];
        switch (m_edge) {
        case Panel::LeftEdge:
            rect = QRect(screen.left(), available.top(), q->width(), available.height());
            struts = {
                q->width(), 0, 0, 0,
                available.top(), available.bottom(), 0, 0,
                0, 0, 0, 0
                };
            break;
        case Panel::TopEdge:
            rect = QRect(screen.left(), screen.top(), screen.width(), q->height());
            struts = {
                0, 0, q->height(), 0,
                0, 0, 0, 0,
                screen.left(), screen.right(), 0, 0
                };
            break;
        }

        q->setGeometry(rect);

        XChangeProperty(QX11Info::display(), q->effectiveWinId(), atom,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) &struts, 12);
    }

    void updateLayoutDirection()
    {
        QBoxLayout::Direction direction;
        switch(m_edge) {
        case Panel::TopEdge:
            direction = QApplication::isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight;
            break;
        case Panel::LeftEdge:
            direction = QBoxLayout::TopToBottom;
            break;
        }
        m_layout->setDirection(direction);
    }

    void updateEdge()
    {
        updateStrut();
        updateLayoutDirection();
    }
};

Panel::Panel(QWidget* parent)
: QWidget(parent)
, d(new PanelPrivate)
{
    d->q = this;
    d->m_edge = Panel::TopEdge;
    d->m_layout = new QHBoxLayout(this);
    d->m_layout->setMargin(0);
    d->m_layout->setSpacing(0);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_Hover);
    setAutoFillBackground(true);
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), SLOT(workAreaResized(int)));
}

Panel::~Panel()
{
    delete d;
}

void Panel::setEdge(Panel::Edge edge)
{
    d->m_edge = edge;
    if (isVisible()) {
        d->updateEdge();
    }
}

Panel::Edge Panel::edge() const
{
    return d->m_edge;
}

void Panel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    d->updateEdge();
}

void Panel::workAreaResized(int screen)
{
    if (this->x11Info().screen() == screen) {
        d->updateEdge();
    }
}

void Panel::paintEvent(QPaintEvent* event)
{
    // Necessary because Oxygen thinks it knows better what to paint in the background
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), palette().brush(QPalette::Background));
}

void Panel::addWidget(QWidget* widget)
{
    d->m_layout->addWidget(widget);
}

void Panel::addSpacer()
{
    d->m_layout->addStretch();
}

} // namespace

#include "panel.moc"
