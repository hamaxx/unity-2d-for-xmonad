/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Local
#include <config.h>

// Applets
#include <appindicator/appindicatorapplet.h>
#include <appmenu/appmenuapplet.h>
#include <appname/appnameapplet.h>
#include <homebutton/homebuttonapplet.h>
#include <indicator/indicatorapplet.h>
#include <legacytray/legacytrayapplet.h>

// Unity
#include <panel.h>

// Qt
#include <QAbstractFileEngineHandler>
#include <QApplication>
#include <QFSFileEngine>
#include <QLabel>

using namespace UnityQt;

class ThemeEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString& fileName) const
    {
        if (fileName.startsWith("theme:")) {
            // FIXME: Do not hardcode path
            QString name = INSTALL_PREFIX "/" THEME_DIR "/" + fileName.mid(6);
            return new QFSFileEngine(name);
        } else {
            return 0;
        }
    }
};

QPalette getPalette()
{
    QPalette palette;

    /* FIXME: I suppose that should be set coming from the theme some how */
    QBrush bg(QPixmap("/usr/share/themes/Ambiance/gtk-2.0/apps/img/panel.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);
    return palette;
}

QLabel* createSeparator()
{
    QLabel* label = new QLabel;
    QPixmap pix("theme:/divider.png");
    label->setPixmap(pix);
    label->setFixedSize(pix.size());
    return label;
}

int main(int argc, char** argv)
{
    ThemeEngineHandler handler;

    QApplication app(argc, argv);
    Panel panel;
    panel.setEdge(Panel::TopEdge);
    panel.setPalette(getPalette());
    panel.setFixedHeight(24);

    panel.addWidget(new HomeButtonApplet);
    panel.addWidget(createSeparator());
    panel.addWidget(new AppNameApplet);
    panel.addWidget(new AppMenuApplet);
    panel.addSpacer();
    panel.addWidget(new LegacyTrayApplet);
    panel.addWidget(new IndicatorApplet);
    panel.show();
    return app.exec();
}
