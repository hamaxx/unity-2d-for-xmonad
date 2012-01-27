/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Alberto Mardegan <alberto.mardegan@canonical.com>
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
#ifndef DASHSETTINGS_H
#define DASHSETTINGS_H

// Qt
#include <QObject>
#include <QSize>

namespace Unity2d {

/**
 * Provide easy access to the Dash settings (from Qt and QML)
 */
class DashSettings: public QObject
{
    Q_OBJECT

public:
    explicit DashSettings(QObject* parent = 0);

    static QSize minimumSizeForDesktop();

private:
    Q_DISABLE_COPY(DashSettings)
};

} // namespace Unity2d

#endif /* DASHSETTINGS_H */
