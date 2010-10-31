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

#include "device.h"

#include "QDebug"

Device::Device() :
    m_volume(NULL)
{
}

Device::Device(const Device& other) :
    m_volume(other.m_volume)
{
    // TODO: connect to the volume's changed signal to monitor name change?
}

Device::~Device()
{
}

bool
Device::active() const
{
    return false;
}

bool
Device::running() const
{
    return false;
}

bool
Device::urgent() const
{
    return false;
}

QString
Device::name() const
{
    if (m_volume != NULL)
        return QString(g_volume_get_name(m_volume));

    return QString("");
}

QString
Device::icon() const
{
    return QString("/usr/share/unity/devices.png");
}

bool
Device::launching() const
{
    // This basically means no launching animation when opening the device.
    // Unity behaves likes this.
    return false;
}

void
Device::activate()
{
    open();
}

GVolume*
Device::getVolume()
{
    return m_volume;
}

void
Device::setVolume(GVolume* volume)
{
    m_volume = volume;
    // TODO: connect to the volume's changed signal to monitor name change?
}

void
Device::open()
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
                       (GAsyncReadyCallback) Device::onVolumeMounted, NULL);
    }
}

void
Device::onVolumeMounted(GVolume* volume, GAsyncResult* res)
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
Device::eject()
{
    if (m_volume == NULL)
        return;

    g_volume_eject_with_operation(m_volume, G_MOUNT_UNMOUNT_NONE, NULL, NULL,
                                  (GAsyncReadyCallback) Device::onVolumeEjected, NULL);
}

void
Device::onVolumeEjected(GVolume* volume, GAsyncResult* res)
{
    g_volume_eject_with_operation_finish(volume, res, NULL);
}

