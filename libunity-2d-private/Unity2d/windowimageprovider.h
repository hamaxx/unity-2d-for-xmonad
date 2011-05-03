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

#ifndef X11WINDOWIMAGEPROVIDER_H
#define X11WINDOWIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QImage>
#include <QSize>

typedef unsigned long Window;

class WindowImageProvider : public QDeclarativeImageProvider
{
public:
    WindowImageProvider();
    ~WindowImageProvider();
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    static void activateComposite();

private:
    QPixmap getWindowPixmap(Window frameWindowId, Window contentWindowId);
    QImage convertWindowPixmap(QPixmap windowPixmap, Window frameWindowId);

    bool m_x11supportsShape;
};

#endif // X11WINDOWIMAGEPROVIDER_H
