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
#include <QCursor>

QLauncherTooltip::QLauncherTooltip(QObject *parent):
    QMainWindow(0, Qt::ToolTip), m_menu(false)
{
    QLabel* label = new QLabel(this);
    label->setMargin(5);
    setCentralWidget(label);

    m_animation = new QPropertyAnimation(this, "size");
    m_animation->setDuration(100);
}

QLauncherTooltip::~QLauncherTooltip()
{
    delete m_animation;
}

void
QLauncherTooltip::show(int y, const QString& name)
{
    if (m_menu)
        return;

    QLabel* label = (QLabel*) centralWidget();
    label->setText(name);

    QDesktopWidget* desktop = QApplication::desktop();
    const QRect available = desktop->availableGeometry(this);
    move(available.x(), y + available.y() - this->sizeHint().height() / 2);
    QMainWindow::show();
    resize(label->minimumSizeHint());
}

void
QLauncherTooltip::hide()
{
    if (m_menu)
    {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect available = desktop->availableGeometry(this);
        QPoint cursor = QCursor::pos();
        if (cursor.x() >= available.x())
            return;
    }

    m_menu = false;
    QMainWindow::hide();
}

void
QLauncherTooltip::show_menu()
{
    QLabel* label = (QLabel*) centralWidget();
    const QSize min = label->minimumSizeHint();

    // FIXME: show an actual contextual menu

    m_animation->setStartValue(min);
    m_animation->setEndValue(QSize(min.width(), min.height() + 100));
    m_animation->start();

    m_menu = true;
}

