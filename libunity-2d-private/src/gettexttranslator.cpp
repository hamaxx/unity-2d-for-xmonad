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
#include "gettexttranslator.h"

// Local
#include <debug_p.h>

// Qt

// libc
#include <libintl.h>

struct GettextTranslatorPrivate
{
};

GettextTranslator::GettextTranslator(QObject* parent)
: QTranslator(parent)
, d(new GettextTranslatorPrivate)
{
}

GettextTranslator::~GettextTranslator()
{
    delete d;
}

QString GettextTranslator::translate(const char* context, const char* sourceText, const char* disambiguation) const
{
    char* text;
    if (qstrlen(disambiguation) > 0) {
        // Constant copied from /usr/share/gettext/gettext.h
        const char GETTEXT_CONTEXT_GLUE = '\004';
        QByteArray array(disambiguation);
        array.append(GETTEXT_CONTEXT_GLUE);
        array.append(sourceText);
        text = gettext(array.constData());
    } else {
        text = gettext(sourceText);
    }
    return QString::fromUtf8(text);
}

bool GettextTranslator::init(const QString& name, const QString& directory)
{
    char* out;
    setlocale(LC_ALL, "");
    out = bindtextdomain(name.toLocal8Bit().constData(), directory.toLocal8Bit().constData());
    if (!out) {
        qWarning("bindtextdomain() failed: %s", strerror(errno));
        return false;
    }

    out = textdomain(name.toLocal8Bit().constData());
    if (!out) {
        qWarning("textdomain() failed: %s", strerror(errno));
        return false;
    }

    return true;
}

#include "gettexttranslator.moc"
