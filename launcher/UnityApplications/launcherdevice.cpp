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

#include <QDebug>

LauncherDevice::LauncherDevice() :
    m_volume(NULL)
{
}

LauncherDevice::LauncherDevice(const LauncherDevice& other) :
    m_volume(other.m_volume)
{
}

LauncherDevice::~LauncherDevice()
{
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

bool
LauncherDevice::urgent() const
{
    return false;
}

QString
LauncherDevice::name() const
{
    if (m_volume != NULL)
        return QString(g_volume_get_name(m_volume));

    return QString("");
}

QString
LauncherDevice::icon() const
{
    return QString("/usr/share/unity/devices.png");
}

bool
LauncherDevice::launching() const
{
    // This basically means no launching animation when opening the device.
    // Unity behaves likes this.
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
}

void
LauncherDevice::open()
{
    if (m_volume == NULL)
        return;

    GMount* mount = g_volume_get_mount(m_volume);
    if (mount != NULL)
    {
        GFile* root = g_mount_get_root(mount);
        char* uri = g_file_get_uri(root);
        GError* error = NULL;
        g_app_info_launch_default_for_uri(uri, NULL, &error);
        if (error != NULL)
        {
            qDebug() << error->message;
        }
        g_free(uri);
        g_object_unref(root);
        g_object_unref(mount);
    }
    else
    {
        if (!g_volume_can_mount(m_volume))
        {
            qDebug() << "Volume cannot be mounted";
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
    if (mount != NULL)
    {
        GFile* root = g_mount_get_root(mount);
        char* uri = g_file_get_uri(root);
        GError* error = NULL;
        g_app_info_launch_default_for_uri(uri, NULL, &error);
        if (error != NULL)
        {
            qDebug() << error->message;
        }
        g_free(uri);
        g_object_unref(root);
        g_object_unref(mount);
    }
    else
    {
        qDebug() << "Unable to mount volume";
    }
}

void
LauncherDevice::eject()
{
    if (m_volume == NULL)
        return;

    g_volume_eject_with_operation(m_volume, G_MOUNT_UNMOUNT_NONE, NULL, NULL,
                                  (GAsyncReadyCallback) LauncherDevice::onVolumeEjected, NULL);
}

void
LauncherDevice::onVolumeEjected(GVolume* volume, GAsyncResult* res)
{
    g_volume_eject_with_operation_finish(volume, res, NULL);
}

void
LauncherDevice::createMenuActions()
{
    m_menu->addSeparator();

    QAction* eject = new QAction(m_menu);
    eject->setText(tr("Eject"));
    m_menu->addAction(eject);
    QObject::connect(eject, SIGNAL(triggered()), this, SLOT(onEjectTriggered()));
}

void
LauncherDevice::onEjectTriggered()
{
    hideMenu(true);
    eject();
}

