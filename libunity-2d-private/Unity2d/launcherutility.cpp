/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

#include "launcherutility.h"

#include <X11/X.h>

extern "C" {
#include <libwnck/libwnck.h>
}

void LauncherUtility::moveViewportToWindow(WnckWindow* window)
{
    WnckWorkspace* workspace = wnck_window_get_workspace(window);
    WnckScreen* screen = wnck_window_get_screen(window);

    int screen_width = wnck_screen_get_width(screen);
    int screen_height = wnck_screen_get_height(screen);
    int viewport_x = wnck_workspace_get_viewport_x(workspace);
    int viewport_y = wnck_workspace_get_viewport_y(workspace);

    int window_x, window_y, window_width, window_height;
    wnck_window_get_geometry(window, &window_x, &window_y,
                                     &window_width, &window_height);

    /* Compute the row and column of the "virtual workspace" that contains
       the window. A "virtual workspace" is a portion of the desktop of the
       size of the screen.
    */
    int viewport_column = (viewport_x + window_x) / screen_width;
    int viewport_row = (viewport_y + window_y) / screen_height;

    wnck_screen_move_viewport(screen, viewport_column * screen_width,
                                      viewport_row * screen_height);
}

void LauncherUtility::showWindow(WnckWindow* window)
{
    WnckWorkspace* workspace = wnck_window_get_workspace(window);

    /* Using X.h's CurrentTime (= 0) */
    wnck_workspace_activate(workspace, CurrentTime);

    /* If the workspace contains a viewport then move the viewport so
       that the window is visible.
       Compiz for example uses only one workspace with a desktop larger
       than the screen size which means that a viewport is used to
       determine what part of the desktop is visible.

       Reference:
       http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#largedesks
    */
    if (wnck_workspace_is_virtual(workspace)) {
        moveViewportToWindow(window);
    }

    /* Using X.h's CurrentTime (= 0) */
    wnck_window_activate(window, CurrentTime);
}

