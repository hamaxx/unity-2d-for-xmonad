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
#include <QMetaType>

namespace Unity2d {

class DashSettingsPrivate;

/**
 * Provide easy access to the Dash settings (from Qt and QML)
 */
class DashSettings: public QObject
{
    Q_OBJECT

    Q_ENUMS(FormFactor)

    Q_PROPERTY(FormFactor formFactor READ formFactor WRITE setFormFactor NOTIFY formFactorChanged)

public:
    explicit DashSettings(QObject* parent = 0);
    ~DashSettings();

    enum FormFactor
    {
        Automatic = 0,
        Desktop,
        Netbook
    };

    void setFormFactor(FormFactor formFactor);
    FormFactor formFactor() const;

Q_SIGNALS:
    void formFactorChanged();

private:
    Q_DISABLE_COPY(DashSettings)
    DashSettingsPrivate* d_ptr;
    Q_DECLARE_PRIVATE(DashSettings)
};

} // namespace Unity2d

Q_DECLARE_METATYPE(Unity2d::DashSettings::FormFactor)

#endif /* DASHSETTINGS_H */
