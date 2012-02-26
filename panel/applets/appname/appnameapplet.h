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

#ifndef APPNAMEAPPLET_H
#define APPNAMEAPPLET_H

class QEvent;

class IndicatorsManager;

// Unity-2d
#include <panelapplet.h>

#include <QtDBus>

struct AppNameAppletPrivate;
class AppNameApplet : public Unity2d::PanelApplet
{
Q_OBJECT
public:
    AppNameApplet(Unity2dPanel* panel);
    ~AppNameApplet();

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void updateWidgets();
    void logReceived(const QDBusMessage &msg);

private:
    Q_DISABLE_COPY(AppNameApplet)
    AppNameAppletPrivate* const d;
    QString xmonadLog;
};

#endif /* APPNAMEAPPLET_H */

