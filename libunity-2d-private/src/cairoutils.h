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
#ifndef CAIROUTILS_H
#define CAIROUTILS_H

// Local
#include <gscopedpointer.h>

// Qt

// Cairo
#include <cairo.h>

class QImage;

namespace CairoUtils {

typedef GScopedPointer<cairo_surface_t, cairo_surface_destroy> SurfacePointer;
typedef GScopedPointer<cairo_t, cairo_destroy> Pointer;

/**
 * Creates a Cairo surface for a QImage.
 * QImage format must be Format_ARGB32_Premultiplied.
 */
cairo_surface_t* createSurfaceForQImage(QImage*);

} // namespace

#endif /* CAIROUTILS_H */
