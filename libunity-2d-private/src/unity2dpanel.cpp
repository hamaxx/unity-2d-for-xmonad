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
#include <indicatorsmanager.h>

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

static const int SLIDE_DURATION = 125;

struct Unity2dPanelPrivate
{
    Unity2dPanel* q;
    Unity2dPanel::Edge m_edge;
    mutable IndicatorsManager* m_indicatorsManager;
    QHBoxLayout* m_layout;
    QPropertyAnimation* m_slideInAnimation;
    QPropertyAnimation* m_slideOutAnimation;
    bool m_useStrut;
    int m_delta;
    bool m_manualSliding;

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

        ulong struts[12] = {};
        switch (m_edge) {
        case Unity2dPanel::LeftEdge:
            if (QApplication::isLeftToRight()) {
                struts[0] = q->width();
                struts[4] = available.top();
                struts[5] = available.y() + available.height();
            } else {
                struts[1] = q->width();
                struts[6] = available.top();
                struts[7] = available.y() + available.height();
            }
            break;
        case Unity2dPanel::TopEdge:
            struts[2] = q->height();
            struts[8] = screen.left();
            struts[9] = screen.x() + screen.width();
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
            if (QApplication::isLeftToRight()) {
                rect = QRect(screen.left(), available.top(), q->width(), available.height());
                rect.moveLeft(m_delta);
            } else {
                rect = QRect(screen.right() - q->width(), available.top(), q->width(), available.height());
                rect.moveRight(screen.right() - m_delta);
            }
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
            direction = QBoxLayout::LeftToRight;
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
};

Unity2dPanel::Unity2dPanel(bool requiresTransparency, QWidget* parent)
: QWidget(parent)
, d(new Unity2dPanelPrivate)
{
    d->q = this;
    d->m_edge = Unity2dPanel::TopEdge;
    d->m_indicatorsManager = 0;
    d->m_useStrut = true;
    d->m_delta = 0;
    d->m_manualSliding = false;
    d->m_layout = new QHBoxLayout(this);
    d->m_layout->setMargin(0);
    d->m_layout->setSpacing(0);

    d->m_slideInAnimation = new QPropertyAnimation(this);
    d->m_slideInAnimation->setTargetObject(this);
    d->m_slideInAnimation->setPropertyName("delta");
    d->m_slideInAnimation->setDuration(SLIDE_DURATION);
    d->m_slideInAnimation->setEndValue(0);

    d->m_slideOutAnimation = new QPropertyAnimation(this);
    d->m_slideOutAnimation->setTargetObject(this);
    d->m_slideOutAnimation->setPropertyName("delta");
    d->m_slideOutAnimation->setDuration(SLIDE_DURATION);
    d->m_slideOutAnimation->setEndValue(-panelSize());

    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_Hover);

    if (QX11Info::isCompositingManagerRunning() && requiresTransparency) {
        setAttribute(Qt::WA_TranslucentBackground);
    } else {
        setAutoFillBackground(true);
    }
    
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

IndicatorsManager* Unity2dPanel::indicatorsManager() const
{
    if (d->m_indicatorsManager == 0)
        d->m_indicatorsManager =
            new IndicatorsManager(const_cast<Unity2dPanel*>(this));

    return d->m_indicatorsManager;
}

void Unity2dPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    d->updateEdge();
    d->m_slideOutAnimation->setEndValue(-panelSize());
}

void Unity2dPanel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    d->m_slideOutAnimation->setEndValue(-panelSize());
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
        Q_EMIT useStrutChanged(value);
    }
}

int Unity2dPanel::delta() const
{
    return d->m_delta;
}

void Unity2dPanel::setDelta(int delta)
{
    /* Clamp delta to be between 0 and minus its size */
    int minDelta = -panelSize();
    int maxDelta = 0;

    d->m_delta = qMax(qMin(delta, maxDelta), minDelta);
    d->updateGeometry();
}

int Unity2dPanel::panelSize() const
{
    return (d->m_edge == Unity2dPanel::TopEdge) ? height() : width();
}

void Unity2dPanel::slideIn()
{
    d->m_slideOutAnimation->stop();
    if (d->m_slideInAnimation->state() != QAbstractAnimation::Running) {
        d->m_slideInAnimation->setStartValue(d->m_delta);
        d->m_slideInAnimation->start();
    }
}

void Unity2dPanel::slideOut()
{
    d->m_slideInAnimation->stop();
    if (d->m_slideOutAnimation->state() != QAbstractAnimation::Running) {
        d->m_slideOutAnimation->setStartValue(d->m_delta);
        d->m_slideOutAnimation->start();
    }
}

bool Unity2dPanel::manualSliding() const
{
    return d->m_manualSliding;
}

void Unity2dPanel::setManualSliding(bool manualSliding)
{
    if (d->m_manualSliding != manualSliding) {
        d->m_manualSliding = manualSliding;
        if (manualSliding) {
            d->m_slideInAnimation->stop();
            d->m_slideOutAnimation->stop();
        }
        Q_EMIT manualSlidingChanged(d->m_manualSliding);
    }
}

#include "unity2dpanel.moc"
