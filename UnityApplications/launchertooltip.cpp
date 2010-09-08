/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "launchertooltip.h"

#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>

QLauncherTooltip::QLauncherTooltip(QObject *parent): QMainWindow(0, Qt::ToolTip)
{
    QLabel* label = new QLabel(this);
    label->setMargin(5);
    setCentralWidget(label);
}

QLauncherTooltip::~QLauncherTooltip()
{
}

void
QLauncherTooltip::show(int y, const QString& name)
{
    QLabel* label = (QLabel*) centralWidget();
    label->setText(name);

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    move(available.x(), y + available.y() - this->sizeHint().height() / 2);
    QMainWindow::show();
    resize(label->minimumSizeHint());
}
