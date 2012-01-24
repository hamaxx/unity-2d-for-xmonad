/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Local
#include <mousearea.h>
#include <unity2dapplication.h>

// Qt
#include <QLabel>
#include <QVBoxLayout>

int main(int argc, char** argv)
{
    Unity2dApplication::earlySetup(argc, argv);
    Unity2dApplication app(argc, argv);
    QWidget window;
    QLabel* enteredLabel = new QLabel("Entered");
    QVBoxLayout* layout = new QVBoxLayout(&window);
    layout->addWidget(enteredLabel);

    MouseArea* area = new MouseArea(&window);
    area->setGeometry(QRect(10, 10, 200, 100));
    QObject::connect(area, SIGNAL(entered()), enteredLabel, SLOT(show()));
    QObject::connect(area, SIGNAL(exited()), enteredLabel, SLOT(hide()));

    enteredLabel->setVisible(area->containsMouse());

    window.show();
    return app.exec();
}
