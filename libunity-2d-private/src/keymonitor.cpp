/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Michał Sawicz <michal.sawicz@canonical.com>
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

// Self
#include "keymonitor.h"

// Qt
#include <QtConcurrentRun>
#include <QDebug>

// X11
#include <X11/Xlib.h>
#include <X11/keysym.h>

// Local
#include <debug_p.h>

#define INVALID_EVENT_TYPE -1

static int key_press_type = INVALID_EVENT_TYPE;
static int notify_type = INVALID_EVENT_TYPE;


KeyMonitor::KeyMonitor(QObject* parent)
: QObject(parent), m_stop(false)
{
    if (this->registerEvents()) {
        getModifiers();
        m_future = QtConcurrent::run(this, &KeyMonitor::run);
    }
}

KeyMonitor::~KeyMonitor()
{
    /* let the running thread know that it should stop */
    m_stop = true;
    m_eventList.clear();
}

KeyMonitor* KeyMonitor::instance()
{
    static KeyMonitor* monitor = new KeyMonitor();
    return monitor;
}


void KeyMonitor::getModifiers()
{
    if(!m_modList.empty()) {
        m_modList.clear();
    }

    XModifierKeymap *xmodmap = XGetModifierMapping(m_display);

    // 8 is for Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, and Mod5
    for (int i=0; i<8*xmodmap->max_keypermod; i++) {
        if (xmodmap->modifiermap[i] > 0 && !m_modList.contains(xmodmap->modifiermap[i])) {
            m_modList.append(xmodmap->modifiermap[i]);
        }
    }
}

bool KeyMonitor::registerEvents()
{
    unsigned long screen;
    Window window;

    XDeviceInfo *devices;
    int num_devices;
    int i, j;

    XDevice *device;
    XInputClassInfo *info;

    XEventClass event_class;

    m_display = XOpenDisplay(NULL);

    screen = DefaultScreen(m_display);
    window = RootWindow(m_display, screen);

    devices = XListInputDevices(m_display, &num_devices);

    for(i=0; i<num_devices; i++) {
        device = XOpenDevice(m_display, devices[i].id);
        if (device == NULL) {
            /* That's not critical since "Virtual core..." devices don't
               allow opening. */
            UQ_DEBUG << "Could not open device: " << devices[i].name;
            continue;
        }

        if (devices[i].use == IsXExtensionKeyboard) {
            for (info=device->classes, j=0; j < device->num_classes; j++, info++) {
                if (info->input_class == KeyClass) {
                    DeviceKeyPress(device, key_press_type, event_class);
                    m_eventList.append(event_class);
                    DeviceMappingNotify(device, notify_type, event_class);
                    m_eventList.append(event_class);
                }
            }
        }
    }

    if (m_eventList.size() == 0) {
        UQ_WARNING << "No input devices found.";
        return false;
    }

    if (XSelectExtensionEvent(m_display, window, m_eventList.data(), m_eventList.size())) {
        UQ_WARNING << "Error selecting events.";
        return false;
    }

    return true;
}


void KeyMonitor::run()
{
    XEvent event;

    while(!m_stop && !XNextEvent(m_display, &event)) {
        if (event.type == key_press_type) {
            XDeviceKeyEvent *keyEvent = (XDeviceKeyEvent *) &event;
            if (!m_modList.contains((KeyCode) keyEvent->keycode)) {
                // if not a modifier
                Q_EMIT keyPressed();
            }
        }
        else if (event.type == notify_type) {
            getModifiers();
        }
    }
}

#include "keymonitor.moc"
