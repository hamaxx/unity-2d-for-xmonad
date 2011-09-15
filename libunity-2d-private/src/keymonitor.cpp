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
#include <QVector>
#include <QtConcurrentRun>
#include <QDebug>

// X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput.h>

// Local
#include <debug_p.h>

#define INVALID_EVENT_TYPE -1

static int key_press_type = INVALID_EVENT_TYPE;
static int notify_type = INVALID_EVENT_TYPE;

struct KeyMonitorPrivate
{
    KeyMonitorPrivate()
    : stop(false)
    { }

    Display *display;
    QFuture<void> future;
    bool stop;
    QVector<XEventClass> event_list;
    QVector<KeyCode> mod_list;
};

KeyMonitor::KeyMonitor(QObject* parent)
: QObject(parent)
, d(new KeyMonitorPrivate)
{
    if (this->register_events()) {
        get_modifiers();
        d->future = QtConcurrent::run(this, &KeyMonitor::run);
    }
}

KeyMonitor::~KeyMonitor()
{
    /* let the running thread know that it should stop */
    d->stop = true;
    d->event_list.clear();
}

KeyMonitor* KeyMonitor::instance()
{
    static KeyMonitor* monitor = new KeyMonitor();
    return monitor;
}


void KeyMonitor::get_modifiers()
{
    if(!d->mod_list.empty()) {
        d->mod_list.clear();
    }

    XModifierKeymap *xmodmap = XGetModifierMapping(d->display);

    // 8 is for Shift, Lock, Control, Mod1, Mod2, Mod3, Mod4, and Mod5
    for (int i=0; i<8*xmodmap->max_keypermod; i++) {
        if (xmodmap->modifiermap[i] > 0 && !d->mod_list.contains(xmodmap->modifiermap[i])) {
            d->mod_list.append(xmodmap->modifiermap[i]);
        }
    }
}

bool KeyMonitor::register_events()
{
    unsigned long screen;
    Window window;

    XDeviceInfo *devices;
    int num_devices;
    int i, j;

    XDevice *device;
    XInputClassInfo *info;

    XEventClass event_class;

    d->display = XOpenDisplay(NULL);

    screen = DefaultScreen(d->display);
    window = RootWindow(d->display, screen);

    devices = XListInputDevices(d->display, &num_devices);

    for(i=0; i<num_devices; i++) {
        device = XOpenDevice(d->display, devices[i].id);
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
                    d->event_list.append(event_class);
                    DeviceMappingNotify(device, notify_type, event_class);
                    d->event_list.append(event_class);
                }
            }
        }
    }

    if (d->event_list.size() == 0) {
        UQ_WARNING << "No input devices found.";
        return false;
    }

    if (XSelectExtensionEvent(d->display, window, d->event_list.data(), d->event_list.size())) {
        UQ_WARNING << "Error selecting events.";
        return false;
    }

    return true;
}


void KeyMonitor::run()
{
    XEvent event;

    while(!d->stop && !XNextEvent(d->display, &event)) {
        if (event.type == key_press_type) {
            XDeviceKeyEvent *key = (XDeviceKeyEvent *) &event;
            if (!d->mod_list.contains((KeyCode) key->keycode)) {
                // if not a modifier
                Q_EMIT keyPressed();
            }
        }
        else if (event.type == notify_type) {
            get_modifiers();
        }
    }
}

#include "keymonitor.moc"
