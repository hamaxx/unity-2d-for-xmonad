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

class DashDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity2d.Dash")
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QString activePlaceEntry READ activePlaceEntry WRITE setActivePlaceEntry NOTIFY activePlaceEntryChanged)
    Q_PROPERTY(QRect screenGeometry READ screenGeometry NOTIFY screenGeometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)

public:
    explicit DashDeclarativeView();

    /* getters */
    bool active() const;
    const QString& activePlaceEntry() const;
    const QRect screenGeometry() const;
    const QRect availableGeometry() const;

    /* setters */
    Q_SLOT void setActive(bool active);
    Q_INVOKABLE void setActivePlaceEntry(const QString& activePlaceEntry);

    /* methods */
    Q_INVOKABLE void activatePlaceEntry(const QString& file, const QString& entry, const int section = 0);
    Q_INVOKABLE void activateHome();

signals:
    void activeChanged(bool);
    void activePlaceEntryChanged(const QString&);

    void screenGeometryChanged();
    void availableGeometryChanged();

public slots:
    void fitToAvailableSpace(int screen);
    void hotkeyTriggered();

private:
    void forceActivateWindow();
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event);

    bool m_active;
    QString m_activePlaceEntry; /* D-Bus object path of the place entry */
};

Q_DECLARE_METATYPE(DashDeclarativeView*)

#endif // DashDeclarativeView_H
