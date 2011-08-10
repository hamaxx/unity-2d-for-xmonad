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
#ifndef GIMAGEUTILS_H
#define GIMAGEUTILS_H

// Local

// Qt

class QImage;
class QString;

struct _GdkPixbuf;
struct _GtkIconTheme;

/**
 * Helper methods to deal with GTK images
 */
namespace GImageUtils
{

QImage imageForIconString(const QString& name, int size, struct _GtkIconTheme* theme = 0);

QImage imageForPixbuf(const struct _GdkPixbuf* pixbuf);

} // namespace

#endif /* GIMAGEUTILS_H */
