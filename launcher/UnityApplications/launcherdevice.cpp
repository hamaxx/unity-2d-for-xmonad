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

#include "launcherdevice.h"
#include "launchermenu.h"

#include "config.h"

#include <QDebug>
#include <QAction>

#undef signals
extern "C" {
    #include <gtk/gtk.h>
}

// libunity-2d
#include <unity2dtr.h>
#include <debug_p.h>

LauncherDevice::LauncherDevice() :
    m_volume(NULL)
{
}

LauncherDevice::LauncherDevice(const LauncherDevice& other)
{
    if (other.m_volume != NULL) {
        setVolume(other.m_volume);
    }
}

LauncherDevice::~LauncherDevice()
{
    if (m_volume != NULL) {
        g_object_unref(m_volume);
    }
}

bool
LauncherDevice::active() const
{
    return false;
}

bool
LauncherDevice::running() const
{
    return false;
}

int
LauncherDevice::windowCount() const
{
    return 0;
}

bool
LauncherDevice::urgent() const
{
    return false;
}

QString
LauncherDevice::name() const
{
    if (m_volume != NULL) {
        char* name = g_volume_get_name(m_volume);
        QString s = QString::fromLocal8Bit(name);
        g_free(name);
        return s;
    }

    return QString("");
}

QString
LauncherDevice::icon() const
{
    return QString(unity2dDirectory() + "/launcher/artwork/devices.png");
}

bool
LauncherDevice::launching() const
{
    /* This basically means no launching animation when opening the device.
       Unity behaves likes this. */
    return false;
}

void
LauncherDevice::activate()
{
    open();
}

GVolume*
LauncherDevice::getVolume()
{
    return m_volume;
}

void
LauncherDevice::setVolume(GVolume* volume)
{
    m_volume = volume;
    g_object_ref(m_volume);
}

void
LauncherDevice::open()
{
    if (m_volume == NULL) {
        return;
    }

    GMount* mount = g_volume_get_mount(m_volume);
    if (mount != NULL) {
        GFile* root = g_mount_get_root(mount);
        char* uri = g_file_get_uri(root);

        GError* error = NULL;
        g_app_info_launch_default_for_uri(uri, NULL, &error);
        if (error != NULL) {
            UQ_WARNING << error->message;
        }

        g_free(uri);
        g_object_unref(root);
        g_object_unref(mount);
    } else {
        if (!g_volume_can_mount(m_volume)) {
            UQ_WARNING << "Volume cannot be mounted";
            return;
        }
        g_volume_mount(m_volume, G_MOUNT_MOUNT_NONE, NULL, NULL,
                       (GAsyncReadyCallback) LauncherDevice::onVolumeMounted, NULL);
    }
}

void
LauncherDevice::onVolumeMounted(GVolume* volume, GAsyncResult* res)
{
    g_volume_mount_finish(volume, res, NULL);
    GMount* mount = g_volume_get_mount(volume);
    if (mount != NULL) {
        GFile* root = g_mount_get_root(mount);
        char* uri = g_file_get_uri(root);

        GError* error = NULL;
        g_app_info_launch_default_for_uri(uri, NULL, &error);
        if (error != NULL) {
            UQ_WARNING << error->message;
        }

        g_free(uri);
        g_object_unref(root);
        g_object_unref(mount);
    } else {
        UQ_WARNING << "Unable to mount volume";
    }
}

void
LauncherDevice::eject()
{
    if (m_volume == NULL) {
        return;
    }

    GMountOperation *mountOperation;
    mountOperation = gtk_mount_operation_new(NULL);

    if (g_volume_can_eject(m_volume)) {
        g_volume_eject_with_operation(m_volume, G_MOUNT_UNMOUNT_NONE, mountOperation,
            NULL, (GAsyncReadyCallback) LauncherDevice::onVolumeEjected, NULL);
    } else {
        GMount* mount = g_volume_get_mount(m_volume);

        if (mount == NULL) {
            return;
        }

        if (g_mount_can_unmount(mount)) {
            g_mount_unmount_with_operation(mount, G_MOUNT_UNMOUNT_NONE, mountOperation,
                NULL, (GAsyncReadyCallback) LauncherDevice::onMountUnmounted,
                NULL);
        } else {
            g_object_unref(mount);
        }
    }

    g_object_unref(mountOperation);
}

void
LauncherDevice::onVolumeEjected(GVolume* volume, GAsyncResult* res)
{
    g_volume_eject_with_operation_finish(volume, res, NULL);
}

void
LauncherDevice::onMountUnmounted(GMount* mount, GAsyncResult* res)
{
    g_mount_unmount_with_operation_finish(mount, res, NULL);
    g_object_unref(mount);
}

void
LauncherDevice::createMenuActions()
{
    QAction* eject = new QAction(m_menu);
    eject->setText(u2dTr("Eject"));
    m_menu->addAction(eject);
    QObject::connect(eject, SIGNAL(triggered()), this, SLOT(onEjectTriggered()));
}

void
LauncherDevice::onEjectTriggered()
{
    m_menu->hide();
    eject();
}

