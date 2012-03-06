/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#ifndef DECAYEDVALUE_H
#define DECAYEDVALUE_H

#include <QObject>

#include <QTimer>

class DecayedValue : public QObject
{
    Q_OBJECT
public:
    DecayedValue();

    bool addAndCheckExceedingTarget(int i);

    void setDecayRate(int decayRate);
    void setTarget(int target);

private Q_SLOTS:
    void decay();

private:
    int m_value;
    int m_target;
    int m_decayRate;
    QTimer m_valueDecayTimer;
};

#endif // DECAYEDVALUE_H
