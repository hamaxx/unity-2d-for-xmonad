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


GestureHandler::GestureHandler(QObject *parent) : QObject(parent)
{
    if (geisInitialize() != GEIS_STATUS_SUCCESS) {
        qWarning("GEIS initialization failed: multitouch support disabled");
        return;
    }

    if (geisStartEventDispatching() != GEIS_STATUS_SUCCESS) {
        qWarning("No file descriptor available for gesture events dispatching");
        return;
    }

    if (geisSubscribeGestures() != GEIS_STATUS_SUCCESS) {
        qWarning("Subscribing to gestures failed: multitouch support disabled");
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


static void toggleDash()
{
    /* A 4 fingers tap will either:
        - show the home page of the dash if the dash is closed
        - close the dash, if the dash is opened
    */
    QDBusInterface dashInterface("com.canonical.Unity2d", "/Dash", "com.canonical.Unity2d.Dash");
    bool dashActive = dashInterface.property("active").toBool();

    if (dashActive) {
        dashInterface.setProperty("active", false);
    } else {
        dashInterface.call(QDBus::Block, "activateHome");
    }
}

static void spreadZoomIn()
{
    /* DOCME */
    /* FIXME: finish me */
    QDBusInterface spreadInterface("com.canonical.Unity2d.Spread", "/Spread", "com.canonical.Unity2d.Spread");
    spreadInterface.call(QDBus::Block, "Hide");
}

static void spreadZoomOut()
{
    /* DOCME */
    /* FIXME: finish me */
    QDBusInterface spreadInterface("com.canonical.Unity2d.Spread", "/Spread", "com.canonical.Unity2d.Spread");
    spreadInterface.call(QDBus::Block, "ShowAllWorkspaces", "");
}

static void gestureFromAttributes(GeisSize attr_count, GeisGestureAttr *attrs, QString& gestureName, int& touches)
{
    /* Look for number of touches and name of gesture in attrs */
    for (int i = 0; i < attr_count; ++i) {
        if (strcmp(attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TOUCHES) == 0) {
            touches = attrs[i].integer_val;
        } else if (strcmp(attrs[i].name, GEIS_GESTURE_ATTRIBUTE_GESTURE_NAME) == 0) {
            gestureName = attrs[i].string_val;
        }
    }
}

static void gestureStart(void *cookie, GeisGestureType type, GeisGestureId id,
                         GeisSize attr_count, GeisGestureAttr *attrs)
{
    int touches = 0;
    QString gestureName;

    gestureFromAttributes(attr_count, attrs, gestureName, touches);

    /* FIXME: finish me */
    if (gestureName == "Pinch" && touches == 3) {
        spreadZoomOut();
    }
}

static void gestureUpdate(void *cookie, GeisGestureType type, GeisGestureId id,
                          GeisSize attr_count, GeisGestureAttr *attrs)
{
    int touches = 0;
    QString gestureName;

    gestureFromAttributes(attr_count, attrs, gestureName, touches);

    if (gestureName == "Tap" && touches == 4) {
        toggleDash();
    } else if (gestureName == "Pinch" && touches == 3) {
        /* FIXME: finish me */
    }
}

static void gestureFinish(void *cookie, GeisGestureType type, GeisGestureId id,
                          GeisSize attr_count, GeisGestureAttr *attrs)
{

}

GeisStatus GestureHandler::geisSubscribeGestures()
{
    /* Subscribe to gestures we are interested in */
    const char* gestures[] = {GEIS_GESTURE_TYPE_TAP4,
                              GEIS_GESTURE_TYPE_PINCH3,
                              NULL};

    m_gestureFuncs.added = NULL;
    m_gestureFuncs.removed = NULL;
    m_gestureFuncs.start = gestureStart;
    m_gestureFuncs.update = gestureUpdate;
    m_gestureFuncs.finish = gestureFinish;

    return geis_subscribe(m_geisInstance, GEIS_ALL_INPUT_DEVICES, gestures,
                          &m_gestureFuncs, NULL);
}
