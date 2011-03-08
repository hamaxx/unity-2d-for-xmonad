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
#include <QCoreApplication>
#include <QTime>

// Glib
#include <glib.h>

// libc
#include <cstdio>
#include <cstdlib>

namespace Unity2dDebug
{

static const char* COLOR_BLUE = "\033[34m";
static const char* COLOR_RED = "\033[31m";

static bool getenvBool(const char* name, bool defaultValue)
{
    QByteArray value = qgetenv(name);
    if (value.isEmpty()) {
        return defaultValue;
    }
    return value != "0";
}

static void unity2dQtHandlerPrint(const char* color, const char* level, const char* message)
{
    static bool useColor = isatty(fileno(stderr)) && getenvBool("UNITY2D_DEBUG_COLOR", true);
    if (useColor) {
        fprintf(stderr, "%s[%s]\033[0m %s\n", color, level, message);
    } else {
        fprintf(stderr, "[%s] %s\n", level, message);
    }
}

static void unity2dQtHandler(QtMsgType type, const char *message)
{
    static QByteArray name = QCoreApplication::applicationFilePath().section("/", -1).toLocal8Bit();
    static bool useTimeStamp = getenvBool("UNITY2D_DEBUG_TIMESTAMP", false);

    // We use fputs here because we don't want stderr to be flushed before the
    // end of the line
    if (useTimeStamp) {
        QString timeStr = QTime::currentTime().toString("HH:mm:ss.zzz: ");
        fputs(qPrintable(timeStr), stderr);
    }

    fputs(name.constData(), stderr);
    fputs(": ", stderr);

    switch (type) {
    case QtDebugMsg:
        unity2dQtHandlerPrint(COLOR_BLUE, "DEBUG", message);
        break;
    case QtWarningMsg:
        unity2dQtHandlerPrint(COLOR_RED, "WARNING", message);
        break;
    case QtCriticalMsg:
        unity2dQtHandlerPrint(COLOR_RED, "CRITICAL", message);
        break;
    case QtFatalMsg:
        unity2dQtHandlerPrint(COLOR_RED, "FATAL", message);
        abort();
    }
}

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

void installHandlers()
{
    g_log_set_default_handler(unity2dGlibHandler, 0);
    qInstallMsgHandler(unity2dQtHandler);
}

} // namespace
