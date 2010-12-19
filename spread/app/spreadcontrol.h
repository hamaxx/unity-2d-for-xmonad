/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
 *  Florian Boucault <florian.boucault@canonical.com>
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

#ifndef SPREADCONTROL_H
#define SPREADCONTROL_H

#include <QObject>
#include <QDBusContext>
#include <QtDeclarative/qdeclarative.h>


class SpreadControl : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit SpreadControl(QObject *parent = 0);
    ~SpreadControl();

    bool connectToBus();

public Q_SLOTS:
    Q_NOREPLY void SpreadAllWindows();
    Q_NOREPLY void SpreadApplicationWindows(unsigned int applicationId);
    Q_NOREPLY void CancelSpread();

Q_SIGNALS:
    void activateSpread(unsigned int applicationId);
    void cancelSpread();
};

QML_DECLARE_TYPE(SpreadControl)

#endif // SPREADCONTROL_H
