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
#include "unity2dpanel.h"
#include <debug_p.h>

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QX11Info>

// X
#include <X11/Xlib.h>
#include <X11/Xatom.h>

static const int SLIDE_DURATION = 500;

struct Unity2dPanelPrivate
{
    Unity2dPanel* q;
    Unity2dPanel::Edge m_edge;
    QHBoxLayout* m_layout;
    QPropertyAnimation* m_slideAnimation;
    bool m_useStrut;
    int m_delta;

    void setStrut(ulong* struts)
    {
        static Atom atom = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
        XChangeProperty(QX11Info::display(), q->effectiveWinId(), atom,
                        XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *) struts, 12);
    }

    void reserveStrut()
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect screen = desktop->screenGeometry(q);
        const QRect available = desktop->availableGeometry(q);

        ulong struts[12];
        switch (m_edge) {
        case Unity2dPanel::LeftEdge:
            struts = {
                q->width(), 0, 0, 0,
                available.top(), available.bottom(), 0, 0,
                0, 0, 0, 0
                };
            break;
        case Unity2dPanel::TopEdge:
            struts = {
                0, 0, q->height(), 0,
                0, 0, 0, 0,
                screen.left(), screen.right(), 0, 0
                };
            break;
        }

        setStrut(struts);
    }

    void releaseStrut()
    {
        ulong struts[12];
        memset(struts, 0, sizeof struts);
        setStrut(struts);
    }

    void updateGeometry()
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect screen = desktop->screenGeometry(q);
        const QRect available = desktop->availableGeometry(q);

        QRect rect;
        switch (m_edge) {
        case Unity2dPanel::LeftEdge:
            rect = QRect(screen.left(), available.top(), q->width(), available.height());
            rect.moveLeft(m_delta);
            break;
        case Unity2dPanel::TopEdge:
            rect = QRect(screen.left(), screen.top(), screen.width(), q->height());
            rect.moveTop(m_delta);
            break;
        }

        q->setGeometry(rect);
    }

    void updateLayoutDirection()
    {
        QBoxLayout::Direction direction;
        switch(m_edge) {
        case Unity2dPanel::TopEdge:
            direction = QApplication::isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight;
            break;
        case Unity2dPanel::LeftEdge:
            direction = QBoxLayout::TopToBottom;
            break;
        }
        m_layout->setDirection(direction);
    }

    void updateEdge()
    {
        if (m_useStrut) {
            reserveStrut();
        }
        updateGeometry();
        updateLayoutDirection();
    }

    void updateAnimation()
    {
        m_slideAnimation->setStartValue(
            m_edge == Unity2dPanel::TopEdge
            ? -q->height()
            : -q->width());
    }
};

Unity2dPanel::Unity2dPanel(QWidget* parent)
: QWidget(parent)
, d(new Unity2dPanelPrivate)
{
    d->q = this;
    d->m_edge = Unity2dPanel::TopEdge;
    d->m_useStrut = true;
    d->m_delta = 0;
    d->m_layout = new QHBoxLayout(this);
    d->m_layout->setMargin(0);
    d->m_layout->setSpacing(0);

    d->m_slideAnimation = new QPropertyAnimation(this);
    d->m_slideAnimation->setTargetObject(this);
    d->m_slideAnimation->setPropertyName("delta");
    d->m_slideAnimation->setDuration(SLIDE_DURATION);
    d->m_slideAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    d->m_slideAnimation->setEndValue(0);
    d->updateAnimation();

    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_Hover);
    setAutoFillBackground(true);
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), SLOT(slotWorkAreaResized(int)));
}

Unity2dPanel::~Unity2dPanel()
{
    if (d->m_useStrut) {
        d->releaseStrut();
    }
    delete d;
}

void Unity2dPanel::setEdge(Unity2dPanel::Edge edge)
{
    d->m_edge = edge;
    if (isVisible()) {
        d->updateEdge();
    }
}

Unity2dPanel::Edge Unity2dPanel::edge() const
{
    return d->m_edge;
}

void Unity2dPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    d->updateEdge();
    d->updateAnimation();
}

void Unity2dPanel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    d->updateAnimation();
}

void Unity2dPanel::slotWorkAreaResized(int screen)
{
    if (x11Info().screen() == screen) {
        d->updateEdge();
    }
}

void Unity2dPanel::paintEvent(QPaintEvent* event)
{
    // Necessary because Oxygen thinks it knows better what to paint in the background
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), palette().brush(QPalette::Background));
}

void Unity2dPanel::addWidget(QWidget* widget)
{
    d->m_layout->addWidget(widget);
}

void Unity2dPanel::addSpacer()
{
    d->m_layout->addStretch();
}

bool Unity2dPanel::useStrut() const
{
    return d->m_useStrut;
}

void Unity2dPanel::setUseStrut(bool value)
{
    if (d->m_useStrut != value) {
        if (value) {
            d->reserveStrut();
        } else {
            d->releaseStrut();
        }
        d->m_useStrut = value;
    }
}

int Unity2dPanel::delta() const
{
    return d->m_delta;
}

void Unity2dPanel::setDelta(int delta)
{
    d->m_delta = delta;
    d->updateGeometry();
}

void Unity2dPanel::slideIn()
{
    d->m_slideAnimation->setDirection(QAbstractAnimation::Forward);
    if (d->m_slideAnimation->state() == QAbstractAnimation::Stopped
        && d->m_delta == d->m_slideAnimation->startValue())
    {
        d->m_slideAnimation->start();
    }
}

void Unity2dPanel::slideOut()
{
    d->m_slideAnimation->setDirection(QAbstractAnimation::Backward);
    if (d->m_slideAnimation->state() == QAbstractAnimation::Stopped && d->m_delta == 0) {
        d->m_slideAnimation->start();
    }
}

#include "unity2dpanel.moc"
