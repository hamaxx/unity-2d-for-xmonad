/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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

#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

// Local

// Qt
#include <QObject>

class QPoint;

struct WindowHelperPrivate;
class WindowHelper : public QObject
{
Q_OBJECT
public:
    WindowHelper(QObject* parent);
    ~WindowHelper();

    void setXid(uint);

    bool isMaximized() const;
    bool isMostlyOnScreen(int screen) const;

public Q_SLOTS:
    void close();
    void minimize();
    void unmaximize();
    void drag(const QPoint& pos);

private Q_SLOTS:
    void update();

Q_SIGNALS:
    void nameChanged();
    void stateChanged();

private:
    WindowHelperPrivate* const d;
};

#endif /* WINDOWHELPER_H */
