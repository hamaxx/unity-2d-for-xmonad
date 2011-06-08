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

#ifndef LAUNCHERMENU_H
#define LAUNCHERMENU_H

#include <QMenu>
#include <QTimer>
#include <QPixmap>

class LauncherItem;

class LauncherContextualMenu : public QMenu
{
    Q_OBJECT

    Q_PROPERTY(bool transparencyAvailable READ transparencyAvailable)
    Q_PROPERTY(bool folded READ folded WRITE setFolded NOTIFY foldedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

public:
    LauncherContextualMenu();
    ~LauncherContextualMenu();

    /* getters */
    bool transparencyAvailable() const;
    bool folded() const;
    LauncherItem* launcherItem() const;
    QString title() const;

    /* setters */
    void setFolded(int folded);
    void setLauncherItem(LauncherItem* launcherItem);
    void setTitle(const QString& title);
    void setVisible(bool value);

    Q_INVOKABLE void show(int x, int y);
    Q_INVOKABLE void hide();
    Q_INVOKABLE void hideWithDelay(int delay);

    void insertActionBeforeTitle(QAction* action);
    QAction* insertSeparatorBeforeTitle();

protected:
    void resizeEvent(QResizeEvent* event);
    void leaveEvent(QEvent* event);
    void enterEvent(QEvent* event);
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);

Q_SIGNALS:
    void foldedChanged(bool);
    void titleChanged(QString);
    void visibleChanged(bool);
    void dismissedByKeyEvent();

public Q_SLOTS:
    void setFocus(); // overridden

private:
    void loadCSS();
    QTimer m_hidingDelayTimer;
    bool m_folded;
    LauncherItem* m_launcherItem;
    QString m_title;
    QAction* m_titleAction;

    QPixmap m_arrow;
    int m_arrowY;

private Q_SLOTS:
    void updateMask();
    void activateWindow();
};

#endif // LAUNCHERMENU_H

