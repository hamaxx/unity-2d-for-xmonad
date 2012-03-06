/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#include "spreadmanager.h"

#include "spreadview.h"

#include <config.h>
#include <screeninfo.h>

#include <QApplication>
#include <QDebug>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeItem>
#include <QDesktopWidget>

SpreadManager::SpreadManager(QObject* parent)
 : QObject(parent)
 , m_grabber(NULL)
{
    QDesktopWidget* desktop = QApplication::desktop();

    onScreenCountChanged(desktop->screenCount());
    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));

    m_control.connectToBus();
}

SpreadManager::~SpreadManager()
{
    qDeleteAll(m_viewList);
}

QObject *SpreadManager::currentSwitcher() const
{
    QWidget *w = qApp->widgetAt(QCursor::pos());
    if (w == NULL) {
        return NULL;
    }

    SpreadView *v = qobject_cast<SpreadView*>(w->window());
    if (v == NULL) {
        return NULL;
    }

    return v->rootObject();
}

SpreadView *SpreadManager::initSpread(int screen)
{
    const QStringList arguments = qApp->arguments();

    SpreadView *view = new SpreadView(screen);
    if (arguments.contains("-opengl")) {
        view->setUseOpenGL(true);
    }

    /* The spread window is borderless and not moveable by the user, yet not
       fullscreen */
    view->setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);

    view->engine()->addImportPath(unity2dImportPath());
    /* Note: baseUrl seems to be picky: if it does not end with a slash,
       setSource() will fail */
    view->engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/spread/"));

    /* Add a SpreadControl instance to the QML context */
    connect(view, SIGNAL(visibleChanged(bool)), this, SLOT(onViewVisibleChanged(bool)));
    view->rootContext()->setContextProperty("control", &m_control);
    view->rootContext()->setContextProperty("spreadManager", this);
    view->rootContext()->setContextProperty("declarativeView", view);

    /* Load the QML UI, focus and show the window */
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->setSource(QUrl("./Workspaces.qml"));

    connect(view->rootObject(), SIGNAL(cancelAndExitStarted()), this, SIGNAL(startCancelAndExit()));

    if (screen == 0) {
        m_grabber = view;
        view->viewport()->installEventFilter(this);
        view->installEventFilter(this);
    }

    return view;
}

void SpreadManager::onScreenCountChanged(int newCount)
{
    const int previousCount = m_viewList.size();

    /* Update the position of other existing Shells, and instantiate new Shells as needed. */
    for (int screen = previousCount; screen < newCount; ++screen) {
        SpreadView *spread = initSpread(screen);
        m_viewList.append(spread);
    }

    /* Remove extra Shells if any. */
    while (m_viewList.size() > newCount) {
        SpreadView *spread = m_viewList.takeLast();
        spread->deleteLater();
    }

    if (newCount == 0) {
        m_grabber = NULL;
    }
}

void SpreadManager::onViewVisibleChanged(bool visible)
{
    m_control.setIsShown(visible);
    if (m_grabber != NULL) {
        m_grabber->forceActivateWindow();
        Q_EMIT currentSwitcherChanged();
    }
}


bool SpreadManager::eventFilter(QObject *obj, QEvent *event) {
    if (m_grabber != NULL) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            {
                QMouseEvent *me = (QMouseEvent*)event;
                const QPoint globalPos = m_grabber->viewport()->mapToGlobal(me->pos());
                QWidget *w = qApp->widgetAt(globalPos);
                if (w == NULL) {
                    if (event->type() == QEvent::MouseButtonPress) {
                        Q_EMIT startCancelAndExit();
                        return true;
                    }
                } else if (w != m_grabber->viewport()) {
                    SpreadView *v = qobject_cast<SpreadView*>(w->window());
                    if (v != NULL) {
                        if (!v->hasFocus()) {
                            v->forceActivateWindow();
                            Q_EMIT currentSwitcherChanged();
                        }
                        QMouseEvent me2(event->type(), w->mapFromGlobal(globalPos), me->button(), me->buttons(), me->modifiers());
                        qApp->sendEvent(w, &me2);
                        return true;
                    } else {
                        qWarning() << "The impossible happened, MouseEvent and window was not a SpreadView";
                    }
                } else {
                    if (!m_grabber->hasFocus()) {
                        m_grabber->forceActivateWindow();
                        Q_EMIT currentSwitcherChanged();
                    }
                }
            }
            break;

            case QEvent::FocusIn:
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
                if (obj == m_grabber && QWidget::mouseGrabber() == NULL) {
                    /* Note that we grab mouse input from the viewport because doing it directly
                    in the view won't work.
                    */
                    m_grabber->viewport()->grabMouse();
                }
            break;

            case QEvent::Hide:
                if (obj == m_grabber && QWidget::mouseGrabber() != NULL) {
                    m_grabber->viewport()->releaseMouse();
                }
            break;

            default:
            break;
        }
    }

    return false;
}
