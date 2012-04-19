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
#include <QSocketNotifier>
#include <QDebug>

// X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

// Local
#include <debug_p.h>
#include <hotmodifier.h>

#define INVALID_EVENT_TYPE -1

static int key_press_type = INVALID_EVENT_TYPE;
static int notify_type = INVALID_EVENT_TYPE;
static int sXkbBaseEventType = 0;

KeyMonitor::KeyMonitor(QObject* parent)
: QObject(parent),
  m_modifiers(Qt::NoModifier)
{
    if (registerEvents()) {
        getModifiers();
    }
}

KeyMonitor::~KeyMonitor()
{
    m_eventList.clear();
    XCloseDisplay(m_display);
}

KeyMonitor* KeyMonitor::instance()
{
    static KeyMonitor* monitor = new KeyMonitor();
    return monitor;
}

Qt::KeyboardModifiers KeyMonitor::keyboardModifiers() const
{
    return m_modifiers;
}

HotModifier *KeyMonitor::getHotModifierFor(Qt::KeyboardModifiers modifiers)
{
    Q_FOREACH(HotModifier* hotModifier, m_hotModifiers) {
        if (hotModifier->modifiers() == modifiers) {
            return hotModifier;
        }
    }

    HotModifier *hotModifier = new HotModifier(modifiers, this);
    m_hotModifiers.append(hotModifier);
    return hotModifier;
}

void KeyMonitor::disableModifiers(Qt::KeyboardModifiers modifiers)
{
    m_disabledModifiers |= modifiers;
    Q_FOREACH(HotModifier* hotModifier, m_hotModifiers) {
        if (hotModifier->modifiers() & m_disabledModifiers) {
            hotModifier->disable();
        }
    }
}

void KeyMonitor::enableModifiers(Qt::KeyboardModifiers modifiers)
{
    Qt::KeyboardModifiers previouslyDisabled = m_disabledModifiers;
    m_disabledModifiers &= ~modifiers;
    Q_FOREACH(HotModifier* hotModifier, m_hotModifiers) {
        if (hotModifier->modifiers() & previouslyDisabled
            && !hotModifier->modifiers() & m_disabledModifiers) {
            hotModifier->onModifiersChanged(keyboardModifiers());
        }
    }
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

    XFreeModifiermap(xmodmap);
}

bool KeyMonitor::registerEvents()
{
    unsigned long screen;
    Window window;

    XDeviceInfo *devices;
    int x11FileDescriptor;
    int num_devices;
    int i, j;

    XDevice *device;
    XInputClassInfo *info;

    XEventClass event_class;

    m_display = XOpenDisplay(NULL);

    screen = DefaultScreen(m_display);
    window = RootWindow(m_display, screen);

    devices = XListInputDevices(m_display, &num_devices);

    /* Check returned list pointer as XListInputDevices may return NULL with num_devices>0, see LP: #965464 */
    if (devices != NULL) {
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
            XCloseDevice(m_display, device);
        }
        XFreeDeviceList(devices);
    }

    if (m_eventList.size() == 0) {
        UQ_WARNING << "No input devices found.";
        return false;
    }

    if (XSelectExtensionEvent(m_display, window, m_eventList.data(), m_eventList.size())) {
        UQ_WARNING << "Error selecting events.";
        return false;
    }

    int opcode, error;
    XkbQueryExtension(m_display, &opcode, &sXkbBaseEventType,  &error, NULL, NULL);
    XkbSelectEvents(m_display, XkbUseCoreKbd, XkbStateNotifyMask, XkbStateNotifyMask);

    /* Dispatch XEvents when there is activity on the X11 file descriptor */
    x11FileDescriptor = ConnectionNumber(m_display);
    QSocketNotifier* socketNotifier = new QSocketNotifier(x11FileDescriptor, QSocketNotifier::Read, this);
    connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(x11EventDispatch()));

    return true;
}

static Qt::KeyboardModifiers qtModifiersFromXcbMods(int xcbModifiers)
{
    Qt::KeyboardModifiers value = Qt::NoModifier;
    if (xcbModifiers & ShiftMask) {
        value |= Qt::ShiftModifier;
    }
    if (xcbModifiers & ControlMask) {
        value |= Qt::ControlModifier;
    }
    if (xcbModifiers & Mod1Mask) {
        value |= Qt::AltModifier;
    }
    if (xcbModifiers & Mod4Mask) {
        value |= Qt::MetaModifier;
    }
    return value;
}

void KeyMonitor::x11EventDispatch()
{
    XEvent event;

    while (XPending(m_display) > 0) {
        XNextEvent(m_display, &event);
        if (event.type == key_press_type) {
            XDeviceKeyEvent *keyEvent = (XDeviceKeyEvent *) &event;
            if (!m_modList.contains((KeyCode) keyEvent->keycode)) {
                // if not a modifier
                Q_EMIT keyPressed();
            }
        }
        else if (event.type == notify_type) {
            getModifiers();
        } else if (event.type == sXkbBaseEventType + XkbEventCode) {
            XkbEvent *xkbEvent = (XkbEvent*)&event;
            if (xkbEvent->any.xkb_type == XkbStateNotify) {
                const Qt::KeyboardModifiers prevMods = m_modifiers;
                m_modifiers = qtModifiersFromXcbMods(xkbEvent->state.mods);
                if (prevMods != m_modifiers) {
                    Q_EMIT keyboardModifiersChanged(m_modifiers);
                    Q_FOREACH(HotModifier* hotModifier, m_hotModifiers) {
                        if (hotModifier->modifiers() & m_disabledModifiers) {
                            /* If any of the modifiers have been disabled, the
                            * hotModifier cannot be triggered */
                            continue;
                        }
                        hotModifier->onModifiersChanged(m_modifiers);
                    }
                }
            }
        }
    }
}

#include "keymonitor.moc"
