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

#include "decayedvalue.h"

DecayedValue::DecayedValue()
 : m_value(0)
 , m_target(0)
 , m_decayRate(0)
{
    m_valueDecayTimer.setInterval(10);
    connect(&m_valueDecayTimer, SIGNAL(timeout()), this, SLOT(decay()));
}

bool DecayedValue::addAndCheckExceedingTarget(int i)
{
    m_value += i;
    if (!m_valueDecayTimer.isActive()) {
        m_valueDecayTimer.start();
    }
    if (m_value > m_target) {
        m_value = 0;
        m_valueDecayTimer.stop();
        return true;
    } else {
        return false;
    }
}

void DecayedValue::setDecayRate(int decayRate)
{
    m_decayRate = decayRate;
}

void DecayedValue::setTarget(int target)
{
    m_target = target;
}

void DecayedValue::decay() {
  const int partial_decay = m_decayRate / 100;

  m_value -= partial_decay;

  if (m_value <= 0)
  {
    m_value = 0;
    m_valueDecayTimer.stop();
  }
}

#include "decayedvalue.moc"