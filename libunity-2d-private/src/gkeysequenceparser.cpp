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

#include "gkeysequenceparser.h"

#include <QDebug>
#include <QString>
#include <QX11Info>

#include <X11/XKBlib.h>

bool GKeySequenceParser::parse(const QString &keySequence, int *x11KeyCode, Qt::KeyboardModifiers *modifiers)
{
    // Parses a string in the form created by the gtk shortcut dialog into x11 keycode and qt modifiers
    // The expected format is
    // <Modifier>*Keyname?
    // i.e. there can be none or multiple modifiers followed or not by the name of a key
    bool success = true;
    *x11KeyCode = 0;
    *modifiers = Qt::NoModifier;
    if (keySequence == "Disabled") {
        return success;
    }

    QString aux = keySequence;
    while (success && aux.startsWith('<')) {
        const int closing = aux.indexOf('>');
        if (closing > 0) {
            const QString modifier = aux.mid(1, closing - 1);
            if (modifier == "Control" || modifier == "Primary") {
                *modifiers = *modifiers | Qt::ControlModifier;
            } else if (modifier == "Shift") {
                *modifiers = *modifiers | Qt::ShiftModifier;
            } else if (modifier == "Alt") {
                *modifiers = *modifiers | Qt::AltModifier;
            } else if (modifier == "Super") {
                *modifiers = *modifiers | Qt::MetaModifier;
            } else {
                qWarning() << "Could not parse modifier" << modifier << "in key sequence" << keySequence;
                success = false;
            }
            aux = aux.mid(closing + 1);
        } else {
            qWarning() << "Could not find modifier end in key sequence" << keySequence;
            success = false;
        }
    }

    if (success && !aux.isEmpty()) {
        KeySym keysym = XStringToKeysym(aux.toLatin1().constData());
        if (keysym == NoSymbol) {
            qWarning() << "Could not parse key" << aux << "in key sequence" << keySequence;
            success = false;
        } else {
            *x11KeyCode = XKeysymToKeycode(QX11Info::display(), keysym);
        }
    }

    return success;
}
