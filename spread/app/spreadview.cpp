/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "spreadview.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QDeclarativeItem>

#include "screeninfo.h"

SpreadView::SpreadView()
: Unity2DDeclarativeView()
{
    m_screenInfo = new ScreenInfo(ScreenInfo::TopLeft, this);
    connect(m_screenInfo, SIGNAL(availableGeometryChanged(QRect)), SLOT(fitToAvailableSpace()));
}

void SpreadView::fitToAvailableSpace()
{
    QRect geometry = m_screenInfo->panelsFreeGeometry();
    setGeometry(geometry);
    setFixedSize(geometry.size());
    rootObject()->setWidth(geometry.width());
    rootObject()->setHeight(geometry.height());
}

/* To be able to call grabMouse() we need to be 100% sure that X11 did
   already map the window. Otherwise grabMouse() will silently fail (and
   confusingly incorrectly reports the widget to be the mouseGrabber())

   Grabbing the mouse just after calling show() on the wiew
   will not work, since the window is really not visible yet.

   One would expect showEvent to be a good candidate, but it doesn't
   work either. According to the docs of QWidget::showEvent the window is
   really shown only when the event's spontaneous() flag is true, however we
   never seem to receive showEvents with this flag set to true.

   The first time the window seems to be actually visible is in focusEvent.
*/
void SpreadView::focusInEvent(QFocusEvent * event)
{
    /* Note that we grab mouse input from the viewport because doing it directly
       in the view won't work.
       It appears that the mouse events are first handled by the vieweport
       and only afterwards by the view itself. If the view grabs all mouse
       input, then the viewport doesn't receive these events anymore and the
       mouse stops working.
       Keyboard events doesn't seem to go the same route and would be unaffected,
       but for consistency let's grab everything from the viewport anyway.
     */
    this->viewport()->installEventFilter(this);
    this->viewport()->grabKeyboard();
    this->viewport()->grabMouse();

    Unity2DDeclarativeView::focusInEvent(event);
}

/* This is not strictly necessary as X11 will cancel any grabs when the
   window is unmapped, and put them back in place when it's mapped again, but
   let's do it anyway for the sake of consistency.
*/
void SpreadView::focusOutEvent(QFocusEvent * event)
{
    this->viewport()->releaseKeyboard();
    this->viewport()->releaseMouse();
    this->viewport()->removeEventFilter(this);

    Unity2DDeclarativeView::focusOutEvent(event);
}

bool SpreadView::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        if (!this->viewport()->geometry().contains(((QMouseEvent*)event)->pos())) {
            Q_EMIT outsideClick();
        }
    }

    return false;
}
