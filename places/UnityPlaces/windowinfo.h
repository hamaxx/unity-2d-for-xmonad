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

#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QtDeclarative/qdeclarative.h>

class BamfWindow;
class BamfApplication;
typedef struct _WnckWindow WnckWindow;

/* This class allows to inspect several pieces of information about a window.
   To know the geometry of the window use the position, size and z properties.
   To know human readable information about the window use icon and title.

   Please note that what is displayed on the screen as a window is in reality
   composed of (at least) two X11 windows: the innermost "content" window, where
   all the actual application widgets live, and the outermost "decorated" window
   where the Window Manager puts any decorations that the window should have
   (like title bar, window buttons, borders, etc.)

   Also please note that all the geometry properties are referred to the
   decorated window, while the title and icon are relative to the content.
*/

class BamfWindow;
class BamfApplication;
typedef struct _WnckWindow WnckWindow;

/* FIXME: position, size, z, title and icon values are not updated real time */
class WindowInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position NOTIFY positionChanged)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(unsigned int z READ z NOTIFY zChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(unsigned int contentXid READ contentXid WRITE setContentXid
                                                       NOTIFY contentXidChanged)
    Q_PROPERTY(unsigned int decoratedXid READ decoratedXid NOTIFY decoratedXidChanged)

public:
    explicit WindowInfo(unsigned int contentXid = 0, QObject *parent = 0);

    /* getters */
    unsigned int decoratedXid() const;
    unsigned int contentXid() const;
    QPoint position() const;
    QSize size() const;
    unsigned int z() const;
    QString title() const;
    QString icon() const;

    /* setters */
    void setContentXid(unsigned int contentXid);

    Q_INVOKABLE void activate();

    /* FIXME: copied from UnityApplications/launcherapplication.h */
    static void showWindow(WnckWindow* window);
    static void moveViewportToWindow(WnckWindow* window);

Q_SIGNALS:
    void contentXidChanged(unsigned int contentXid);
    void decoratedXidChanged(unsigned int decoratedXid);
    void positionChanged(QPoint position);
    void sizeChanged(QSize size);
    void zChanged(unsigned int z);
    void titleChanged(QString title);
    void iconChanged(QString icon);

private:
    void updateGeometry();
    BamfWindow* getBamfWindowForApplication(BamfApplication *application, unsigned int xid);
    WnckWindow* getWnckWindowForXid(unsigned int xid);
    unsigned int findTopmostAncestor(unsigned int xid);

    BamfApplication *m_bamfApplication;
    BamfWindow *m_bamfWindow;
    WnckWindow *m_wnckWindow;
    unsigned int m_contentXid;
    unsigned int m_decoratedXid;
    QPoint m_position;
    QSize m_size;
};

QML_DECLARE_TYPE(WindowInfo)

#endif // WINDOWINFO_H
