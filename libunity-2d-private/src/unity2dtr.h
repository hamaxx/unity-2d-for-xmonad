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
#ifndef UNITY2DTR_H
#define UNITY2DTR_H

class QString;

namespace Unity2dTr
{

/**
 * Installs our gettext catalog
 */
void init(const char* domain, const char* localeDir);

} // namespace

/**
 * Translate the string text
 */
QString u2dTr(const char* text);

/**
 * Plural version of utr. Should be called like this:
 *
 * u2dTr("%1 file", "%1 files", count)
 */
QString u2dTr(const char* singular, const char* plural, int n);

#endif /* UNITY2DTR_H */
