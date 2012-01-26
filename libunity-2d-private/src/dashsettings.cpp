/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Alberto Mardegan <alberto.mardegan@canonical.com>
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
#include "dashsettings.h"

#include <config.h>

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

namespace Unity2d {

DashSettings::DashSettings(QObject* parent):
    QObject(parent)
{
}

static int getenvInt(const char* name, int defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    int value = stringValue.toInt(&ok);
    return ok ? value : defaultValue;
}

QSize DashSettings::minimumSizeForDesktop()
{
    static int minWidth = getenvInt("DASH_MIN_SCREEN_WIDTH",
                                    DASH_MIN_SCREEN_WIDTH);
    static int minHeight = getenvInt("DASH_MIN_SCREEN_HEIGHT",
                                     DASH_MIN_SCREEN_HEIGHT);
    return QSize(minWidth, minHeight);
}

}; // namespace Unity2d

#include "dashsettings.moc"
