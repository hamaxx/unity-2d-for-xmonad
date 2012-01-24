/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QHash>

class IconImageProvider : public QDeclarativeImageProvider
{
public:
    IconImageProvider();
    ~IconImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    /* Cache of Gtk themes */
    /* FIXME: values are set to be of type void* in order to avoid importing
              gtk/gtk.h necessary for GtkIconTheme that would otherwise lead
              to conflicts with Qt keywords (signals, slots, etc.).
    */
    QHash<QString, void*> m_themes;
};

#endif // ICONIMAGEPROVIDER_H
