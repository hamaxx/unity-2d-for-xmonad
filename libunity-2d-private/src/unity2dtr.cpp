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
#include <unity2dtr.h>

// local
#include <config.h>

// Qt
#include <QDeclarativeContext>

// libc
#include <libintl.h>

namespace Unity2dTr
{

void init(const char* domain, const char* localeDir)
{
    setlocale(LC_ALL, "");
    bindtextdomain(domain, localeDir);
    textdomain(domain);
}

void qmlInit(QDeclarativeContext* context)
{
    static QmlHelper helper;
    context->setContextProperty("u2d", &helper);
}

QString QmlHelper::tr(const QString& text)
{
    return ::u2dTr(text.toUtf8().constData());
}

QString QmlHelper::tr(const QString& singular, const QString& plural, int n)
{
    return ::u2dTr(
        singular.toUtf8().constData(),
        plural.toUtf8().constData(),
        n);
}

} // namespace

QString u2dTr(const char* text)
{
    return QString::fromUtf8(gettext(text));
}

QString u2dTr(const char* singular, const char* plural, int n)
{
    QString text = QString::fromUtf8(ngettext(singular, plural, n));
    // Note: if `text` is "%%n" (meaning the string on screen should be "%n"
    // literally), this will fail. I think we don't care for now.
    text.replace("%n", QString::number(n));
    return text;
}

#include <unity2dtr.moc>
