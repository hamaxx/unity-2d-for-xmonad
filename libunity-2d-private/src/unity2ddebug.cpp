/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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
#include "unity2ddebug.h"

// Local

// Qt

// Glib
#include <glib.h>

namespace Unity2dDebug
{

static void unity2dGlibHandler(const gchar* domain, GLogLevelFlags level, const gchar* message, gpointer /* user_data */)
{
    switch (level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
        qFatal("%s: %s", domain, message);
        break;
    case G_LOG_LEVEL_CRITICAL:
        qCritical("%s: %s", domain, message);
        break;
    case G_LOG_LEVEL_WARNING:
        qWarning("%s: %s", domain, message);
        break;
    case G_LOG_LEVEL_MESSAGE:
    case G_LOG_LEVEL_INFO:
    case G_LOG_LEVEL_DEBUG:
        qDebug("%s: %s", domain, message);
        break;
    default:
        qWarning("(Unknown level value %d!) %s: %s", int(level), domain, message);
        break;
    }

    if (level & G_LOG_FLAG_FATAL) {
        // Could happen if g_log_set_fatal_mask() has been called.
        abort();
    }
}

void installGlibHandler()
{
    g_log_set_default_handler(unity2dGlibHandler, 0);
}

} // namespace
