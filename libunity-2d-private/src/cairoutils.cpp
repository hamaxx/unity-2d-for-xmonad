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
#include "cairoutils.h"

// Local

// Qt
#include <QImage>

namespace CairoUtils {

cairo_surface_t* createSurfaceForQImage(QImage* image)
{
    return cairo_image_surface_create_for_data(
        image->bits(),
        CAIRO_FORMAT_ARGB32,
        image->width(),
        image->height(),
        image->bytesPerLine()
        );
}

} // namespace
