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
class BamfView;
typedef struct _WnckWindow WnckWindow;
typedef void* gpointer;

/* FIXME: position, size, z, title and icon values are not updated real time */
class WindowInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(RoleNames)

    Q_PROPERTY(QPoint position READ position NOTIFY positionChanged)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(unsigned int z READ z NOTIFY zChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(unsigned int contentXid READ contentXid WRITE setContentXid
                                                       NOTIFY contentXidChanged)
    Q_PROPERTY(unsigned int decoratedXid READ decoratedXid NOTIFY decoratedXidChanged)
    Q_PROPERTY(QString desktopFile READ desktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(int workspace READ workspace WRITE setWorkspace NOTIFY workspaceChanged)

public:
    explicit WindowInfo(unsigned int contentXid = 0, QObject *parent = 0);
    ~WindowInfo();

    /* getters */
    unsigned int decoratedXid() const;
    unsigned int contentXid() const;
    QPoint position() const;
    QSize size() const;
    unsigned int z() const;
    QString title() const;
    QString icon() const;
    QString desktopFile() const;
    int workspace() const;

    /* setters */
    void setContentXid(unsigned int contentXid);
    void setWorkspace(int workspaceNumber);

    Q_INVOKABLE void activate();

    static void showWindow(WnckWindow* window);

    enum RoleNames {
        RoleWindowInfo,
        RoleDesktopFile,
        RoleWorkspace
    };

Q_SIGNALS:
    void contentXidChanged(unsigned int contentXid);
    void decoratedXidChanged(unsigned int decoratedXid);
    void positionChanged(QPoint position);
    void sizeChanged(QSize size);
    void zChanged(unsigned int z);
    void titleChanged(QString title);
    void iconChanged(QString icon);
    void desktopFileChanged(QString desktopFile);
    void workspaceChanged(int workspace);

private:
    void updateGeometry();
    void updateWorkspace();
    BamfWindow* getBamfWindowForApplication(BamfApplication *application, unsigned int xid);
    WnckWindow* getWnckWindowForXid(unsigned int xid);
    unsigned int findTopmostAncestor(unsigned int xid);
    bool isSameBamfWindow(BamfWindow *other);
    static void onWorkspaceChanged(WnckWindow *window, gpointer user_data);

private:
    BamfApplication *m_bamfApplication;
    BamfWindow *m_bamfWindow;
    WnckWindow *m_wnckWindow;
    unsigned int m_contentXid;
    unsigned int m_decoratedXid;
    QPoint m_position;
    QSize m_size;

/* This is needed so that WindowsList can access isSameBamfWindow.
   Check WindowInfo::removeWindow for an explanation of why. */
friend class WindowsList;
};

QML_DECLARE_TYPE(WindowInfo)

#endif // WINDOWINFO_H
