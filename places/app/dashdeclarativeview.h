/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef DashDeclarativeView_H
#define DashDeclarativeView_H

#include <QDeclarativeView>
#include <unity2ddeclarativeview.h>

class LauncherClient;

class DashDeclarativeView : public Unity2DDeclarativeView
{
    Q_OBJECT
    Q_ENUMS(DashMode)

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(DashMode dashMode READ dashMode WRITE setDashMode NOTIFY dashModeChanged)
    Q_PROPERTY(QString activeLens READ activeLens WRITE setActiveLens NOTIFY activeLensChanged)
    Q_PROPERTY(QRect screenGeometry READ screenGeometry NOTIFY screenGeometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)

public:
    enum DashMode {
        DesktopMode,
        FullScreenMode
    };
    explicit DashDeclarativeView();

    /* getters */
    bool active() const;
    DashMode dashMode() const;
    const QString& activeLens() const;
    const QRect screenGeometry() const;
    QRect availableGeometry() const;
    bool expanded() const;

    /* setters */
    Q_SLOT void setActive(bool active);
    Q_INVOKABLE void setDashMode(DashMode);
    Q_INVOKABLE void setActiveLens(const QString& activeLens);
    Q_INVOKABLE void setExpanded(bool);

    /* methods */
    Q_INVOKABLE void activateLens(const QString& lensId);
    Q_SLOT void activateHome();
    bool connectToBus();

Q_SIGNALS:
    void activeChanged(bool);
    void dashModeChanged(DashMode);
    void expandedChanged(bool);
    void activeLensChanged(const QString&);

    void screenGeometryChanged();
    void availableGeometryChanged();

protected:
    void resizeEvent(QResizeEvent*);
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void onWorkAreaResized(int screen);
    void updateDashModeDependingOnScreenGeometry();
    void updateSize();

private:
    void fitToAvailableSpace();
    void resizeToDesktopModeSize();
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void updateMask();
    void setWMFlags();

    LauncherClient* m_launcherClient;
    DashMode m_mode;
    bool m_expanded;
    QString m_activeLens; /* Lens id of the active lens */
    bool m_active;
};

Q_DECLARE_METATYPE(DashDeclarativeView*)

#endif // DashDeclarativeView_H
