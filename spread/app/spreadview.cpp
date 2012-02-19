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

/*
 * Modified by:
 * - Jure Ham <jure@hamsworld.net>
 */

#include "spreadview.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>

#include "launcherclient.h"

#include <debug_p.h>

// unity-2d
#include <launcherclient.h>

SpreadView::SpreadView() : Unity2DDeclarativeView()
{
}

void SpreadView::fitToAvailableSpace()
{
    int currentScreen = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenRect = QApplication::desktop()->screenGeometry(currentScreen);
    QRect availableRect = QApplication::desktop()->availableGeometry(currentScreen);
    QRect availableGeometry;

    if (currentScreen == QApplication::desktop()->primaryScreen()) {
        availableGeometry = QRect(
            LauncherClient::MaximumWidth,
            screenRect.top() + 24,
            screenRect.width() - LauncherClient::MaximumWidth - 2,
            availableRect.height() - 26
            );
    } else {
        availableGeometry = QRect(
            screenRect.left(),
            screenRect.top(),
            screenRect.width() - 2,
            availableRect.height() - 2
            );
    }
    move(availableGeometry.topLeft());
    setFixedSize(availableGeometry.size());
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

    QDeclarativeView::focusInEvent(event);
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

    QDeclarativeView::focusOutEvent(event);
}

bool SpreadView::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QPoint pos = ((QMouseEvent*)event)->pos();
        QRect rect = this->viewport()->geometry();
        rect.adjust(-1, -1, 2, 2); //border
        if (!rect.contains(pos, false) && QCursor().pos().x() > LauncherClient::MaximumWidth) {
            Q_EMIT outsideClick();
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
		if (!this->viewport()->geometry().contains(((QMouseEvent*)event)->pos())) {
			Q_EMIT outsideClick();
		}
	}

    return false;
}

void SpreadView::showEvent(QShowEvent *event)
{
	fitToAvailableSpace(); //always adjust size
    Q_UNUSED(event);
    Q_EMIT visibleChanged(true);
}

void SpreadView::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    Q_EMIT visibleChanged(false);
}
