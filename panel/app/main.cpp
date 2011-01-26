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

// Local
#include <config.h>

// Applets
#include <appindicator/appindicatorapplet.h>
#include <appname/appnameapplet.h>
#include <homebutton/homebuttonapplet.h>
#include <indicator/indicatorapplet.h>
#include <legacytray/legacytrayapplet.h>

// Unity
#include <unity2dpanel.h>
#include <unity2dapplication.h>
#include <unity2dstyle.h>

// Qt
#include <QAbstractFileEngineHandler>
#include <QApplication>
#include <QFSFileEngine>
#include <QLabel>

using namespace Unity2d;

class ThemeEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString& fileName) const
    {
        if (fileName.startsWith("theme:")) {
            QString name = UNITY_DIR "themes/" + fileName.mid(6);
            return new QFSFileEngine(name);
        } else {
            return 0;
        }
    }
};

QPalette getPalette()
{
    QPalette palette;

    /* Should use the panel's background provided by Unity but it turns
       out not to be good. It would look like:

         QBrush bg(QPixmap("theme:/panel_background.png"));
    */
    QBrush bg(QPixmap(unity2dDirectory() + "/panel/artwork/background.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);
    return palette;
}

QLabel* createSeparator()
{
    QLabel* label = new QLabel;
    QPixmap pix(unity2dDirectory() + "/panel/artwork/divider.png");
    label->setPixmap(pix);
    label->setFixedSize(pix.size());
    return label;
}

int main(int argc, char** argv)
{
    ThemeEngineHandler handler;

    /* Forcing graphics system to 'raster' instead of the default 'native'
       which on X11 is 'XRender'.
       'XRender' defaults to using a TrueColor visual. We mimick that behaviour
       with 'raster' by calling QApplication::setColorSpec.

       Reference: https://bugs.launchpad.net/upicek/+bug/674484
    */
    QApplication::setGraphicsSystem("raster");
    QApplication::setColorSpec(QApplication::ManyColor);
    Unity2dApplication app(argc, argv);
    QApplication::setStyle(new Unity2dStyle);
    Unity2dPanel panel;
    panel.setEdge(Unity2dPanel::TopEdge);
    panel.setPalette(getPalette());
    panel.setFixedHeight(24);

    panel.addWidget(new HomeButtonApplet);
    panel.addWidget(createSeparator());
    panel.addWidget(new AppNameApplet);
    panel.addWidget(new LegacyTrayApplet);
    panel.addWidget(new IndicatorApplet);
    panel.show();
    return app.exec();
}
