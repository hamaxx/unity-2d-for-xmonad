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
#ifndef GETTEXTTRANSLATOR_H
#define GETTEXTTRANSLATOR_H

// Local

// Qt
#include <QTranslator>

struct GettextTranslatorPrivate;
/**
 * A QTranslator which uses gettext
 */
class GettextTranslator : public QTranslator
{
    Q_OBJECT
public:
    GettextTranslator(QObject* parent = 0);
    ~GettextTranslator();

    /**
     * Reimplemented. Note that context is ignored.
     */
    QString translate(const char* context, const char* sourceText, const char* disambiguation=0) const;

    /**
     * Initialize the translator.
     * Translations are then loaded from:
     * $directory/$lang/locale/LC_MESSAGES/$domainName.mo
     */
    bool init(const QString& domainName, const QString& directory);

private:
    Q_DISABLE_COPY(GettextTranslator)
    GettextTranslatorPrivate* const d;

    // Hide "load" methods because they are used to load .qm
    bool load(const QString& filename, const QString& directory = QString(), const QString& search_delimiters = QString(), const QString& suffix = QString());
    bool load(const uchar* data, int len);
};

#endif /* GETTEXTTRANSLATOR_H */
