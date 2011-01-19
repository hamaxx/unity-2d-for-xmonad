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

#ifndef DATETIMEINDICATOR_H
#define DATETIMEINDICATOR_H

// Local
#include "abstractindicator.h"

// Qt
#include <QTimer>

class DateTimeIndicator : public AbstractIndicator
{
    Q_OBJECT
public:
    DateTimeIndicator(QObject* parent=0);

    virtual void init();

private Q_SLOTS:
    void updateText();
private:
    Q_DISABLE_COPY(DateTimeIndicator)
    QAction* m_action;
    QTimer* m_timer;
    QString m_format;

    void setupTimer();
    void setupMenu();
    void readConfig();
    void updateTimer();
};

#endif /* DATETIMEINDICATOR_H */
