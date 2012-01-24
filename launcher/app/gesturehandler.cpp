/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "gesturehandler.h"

#include <QX11Info>
#include <QSocketNotifier>
#include <QDBusInterface>

#include <debug_p.h>

GestureHandler::GestureHandler(Unity2dPanel* launcher, QObject *parent) : QObject(parent), m_launcher(launcher)
{
    if (geisInitialize() != GEIS_STATUS_SUCCESS) {
        UQ_WARNING << "GEIS initialization failed: multitouch support disabled";
        return;
    }

    if (geisStartEventDispatching() != GEIS_STATUS_SUCCESS) {
        UQ_WARNING << "No file descriptor available for gesture events dispatching";
        return;
    }

    if (geisSubscribeGestures() != GEIS_STATUS_SUCCESS) {
        UQ_WARNING << "Subscribing to gestures failed: multitouch support disabled";
    }
}

GestureHandler::~GestureHandler()
{
    geis_finish(m_geisInstance);
}

GeisStatus GestureHandler::geisInitialize()
{
    /* Initialize GEIS to receive gesture events wherever they happen */
    GeisXcbWinInfo geisXcbWinInfo;
    geisXcbWinInfo.display_name = NULL;
    geisXcbWinInfo.screenp = NULL;
    geisXcbWinInfo.window_id = QX11Info::appRootWindow();

    GeisWinInfo geisWinInfo;
    geisWinInfo.win_type = GEIS_XCB_FULL_WINDOW;
    geisWinInfo.win_info = &geisXcbWinInfo;

    return geis_init(&geisWinInfo, &m_geisInstance);
}

GeisStatus GestureHandler::geisStartEventDispatching()
{
    GeisStatus status;
    int geisFileDescriptor;

    /* Dispatch GEIS events when there is activity on geisFileDescriptor */
    status = geis_configuration_supported(m_geisInstance, GEIS_CONFIG_UNIX_FD);
    if (status != GEIS_STATUS_SUCCESS) {
        return status;
    }

    status = geis_configuration_get_value(m_geisInstance, GEIS_CONFIG_UNIX_FD, &geisFileDescriptor);
    if (status != GEIS_STATUS_SUCCESS) {
        return status;
    }

    QSocketNotifier* socketNotifier = new QSocketNotifier(geisFileDescriptor, QSocketNotifier::Read, this);
    connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(geisEventDispatch()));

    return status;
}


void GestureHandler::geisEventDispatch()
{
    geis_event_dispatch(m_geisInstance);
}

GeisStatus GestureHandler::geisSubscribeGestures()
{
    /* Subscribe to gestures we are interested in */
    const char* gestures[] = {GEIS_GESTURE_TYPE_TAP4,
                              GEIS_GESTURE_TYPE_PINCH3,
                              GEIS_GESTURE_TYPE_DRAG4,
                              NULL};

    /* Listening to added/removed gesture is unhelpful with GEIS 1.0 as the
       GeisGestureType received there are different from the ones received
       in start/update/finish
    */
    m_gestureFuncs.added = NULL;
    m_gestureFuncs.removed = NULL;
    m_gestureFuncs.start = GestureHandler::staticGestureStart;
    m_gestureFuncs.update = GestureHandler::staticGestureUpdate;
    m_gestureFuncs.finish = GestureHandler::staticGestureFinish;

    return geis_subscribe(m_geisInstance, GEIS_ALL_INPUT_DEVICES, gestures,
                          &m_gestureFuncs, this);
}


static void toggleDash()
{
    /* A 4 fingers tap will either:
        - show the home page of the dash if the dash is closed
        - close the dash, if the dash is opened
    */
    QDBusInterface dashInterface("com.canonical.Unity2d.Dash", "/Dash", "com.canonical.Unity2d.Dash");
    bool dashActive = dashInterface.property("active").toBool();

    if (dashActive) {
        dashInterface.setProperty("active", false);
    } else {
        dashInterface.call(QDBus::Block, "activateHome");
    }
}

/* FIXME: zooming in/out in the spread should have 3 levels:
    1) showing all windows of the focused application in the current workspace
    2) showing all windows in the current workspace
    3) showing all windows in all workspaces

    This will require changes to the workspace switcher (spread) to advertise its current state over D-Bus.
*/
static void spreadZoomIn()
{
    QDBusInterface spreadInterface("com.canonical.Unity2d.Spread", "/Spread", "com.canonical.Unity2d.Spread");
    spreadInterface.call(QDBus::Block, "Hide");
}

static void spreadZoomOut()
{
    QDBusInterface spreadInterface("com.canonical.Unity2d.Spread", "/Spread", "com.canonical.Unity2d.Spread");
    spreadInterface.call(QDBus::Block, "ShowAllWorkspaces", "");
}



/* Return a dictionary of attribute name -> attribute value */
QHash<QString, GeisGestureAttr> GestureHandler::parseGestureAttributes(GeisSize attr_count, GeisGestureAttr *attrs)
{
    QHash<QString, GeisGestureAttr> parsedAttributes;
    GeisGestureAttr attribute;
    QString attributeName;

    for (unsigned int i = 0; i < attr_count; ++i) {
        attribute = attrs[i];
        attributeName = attribute.name;
        parsedAttributes[attributeName] = attribute;
    }

    return parsedAttributes;
}

/* Static methods used as callbacks for GEIS and that only forward the call to
   non static methods of the GestureHandler instance passed as first parameter */
void GestureHandler::staticGestureStart(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                        GeisSize attr_count, GeisGestureAttr *attrs)
{
    QHash<QString, GeisGestureAttr> attributes = parseGestureAttributes(attr_count, attrs);
    ((GestureHandler*)gestureHandler)->gestureStart(type, id, attributes);
}

void GestureHandler::staticGestureUpdate(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                         GeisSize attr_count, GeisGestureAttr *attrs)
{
    QHash<QString, GeisGestureAttr> attributes = parseGestureAttributes(attr_count, attrs);
    ((GestureHandler*)gestureHandler)->gestureUpdate(type, id, attributes);
}

void GestureHandler::staticGestureFinish(void *gestureHandler, GeisGestureType type, GeisGestureId id,
                                         GeisSize attr_count, GeisGestureAttr *attrs)
{
    QHash<QString, GeisGestureAttr> attributes = parseGestureAttributes(attr_count, attrs);
    ((GestureHandler*)gestureHandler)->gestureFinish(type, id, attributes);
}


/* Gesture event handlers */
void GestureHandler::gestureStart(GeisGestureType type, GeisGestureId id,
                                  QHash<QString, GeisGestureAttr> attributes)
{
    QString gestureName = attributes[GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME].string_val;

    if (gestureName == GEIS_GESTURE_TYPE_PINCH3) {
        /* 3 fingers pinch inwards shows the workspace switcher (zoom out showing all workspaces)
           3 fingers pinch outwards (also called 'spread' by designers) hides the workspace switcher (zoom in a workspace)
         */
        float radiusDelta = attributes[GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA].float_val;
        if (radiusDelta < 0) {
            spreadZoomOut();
        } else if (radiusDelta > 0) {
            spreadZoomIn();
        }

        m_pinchPreviousRadius = attributes[GEIS_GESTURE_ATTRIBUTE_RADIUS].float_val;
        m_pinchPreviousTimestamp = attributes[GEIS_GESTURE_ATTRIBUTE_TIMESTAMP].integer_val;
    } else if (gestureName == GEIS_GESTURE_TYPE_DRAG4) {
        /* 4 fingers drag reveals the launcher progressively; if the drag goes far
           enough, the launcher is then locked in place and does not autohide anymore */
        /* FIXME: only supports the launcher positioned on the left edge of the screen */
        m_launcher->setManualSliding(true);
        m_dragDelta = m_launcher->delta() + attributes[GEIS_GESTURE_ATTRIBUTE_DELTA_X].float_val;
        m_launcher->setDelta(m_dragDelta);
    }
}

void GestureHandler::gestureUpdate(GeisGestureType type, GeisGestureId id,
                                   QHash<QString, GeisGestureAttr> attributes)
{
    QString gestureName = attributes[GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME].string_val;

    if (gestureName == GEIS_GESTURE_TYPE_TAP4) {
        /* 4 fingers tap toggles the dash on and off */
        toggleDash();
    } else if (gestureName == GEIS_GESTURE_TYPE_PINCH3) {
        /* Continuing a 3 fingers pinch inwards/outwards shows/hides the workspace switcher. */
        int timestamp = attributes[GEIS_GESTURE_ATTRIBUTE_TIMESTAMP].integer_val;
        float radius = attributes[GEIS_GESTURE_ATTRIBUTE_RADIUS].float_val;

        /* Ignore pinch events that are too close in time from the previous
           pinching event that triggered an action as they are likely to be a
           continuation part of the previous pinch event.
        */
        if (timestamp - m_pinchPreviousTimestamp < 500) {
            m_pinchPreviousRadius = radius;
            m_pinchPreviousTimestamp = timestamp;
            return;
        }

        /* For the pinch event to trigger an action the difference in radius between
           the current pinch event and the last pinch event that triggered an action
           has to be enough as to not be over sensitive and to not trigger actions for
           any small movement of the user's fingers.
        */
        if (radius - m_pinchPreviousRadius > 30) {
            spreadZoomIn();
            m_pinchPreviousRadius = radius;
            m_pinchPreviousTimestamp = timestamp;
        } else if (radius - m_pinchPreviousRadius < -30) {
            spreadZoomOut();
            m_pinchPreviousRadius = radius;
            m_pinchPreviousTimestamp = timestamp;
        }
    } else if (gestureName == GEIS_GESTURE_TYPE_DRAG4) {
        /* FIXME: only supports the launcher positioned on the left edge of the screen */
        m_dragDelta += attributes[GEIS_GESTURE_ATTRIBUTE_DELTA_X].float_val;
        m_launcher->setDelta(m_dragDelta);
        /* If the drag goes sufficiently above than the maximum delta then
           lock the launcher in place by reserving the area so that no windows
           overlap it.
         */
        if (m_dragDelta - m_launcher->delta() > 240) {
            m_launcher->setUseStrut(true);
        } else {
            m_launcher->setUseStrut(false);
        }
    }
}

void GestureHandler::gestureFinish(GeisGestureType type, GeisGestureId id,
                                   QHash<QString, GeisGestureAttr> attributes)
{
    QString gestureName = attributes[GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME].string_val;

    if (gestureName == GEIS_GESTURE_TYPE_DRAG4) {
        m_dragDelta += attributes[GEIS_GESTURE_ATTRIBUTE_DELTA_X].float_val;
        m_launcher->setDelta(m_dragDelta);
        m_launcher->setManualSliding(false);
    }
}

