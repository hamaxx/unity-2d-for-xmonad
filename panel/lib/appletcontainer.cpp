/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: LGPL v3
 */
// Self
#include "appletcontainer.h"

// Local
#include "applet.h"
#include "config.h"

// Qt
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QX11Info>

// X
#include <X11/Xutil.h>

namespace UnityQt
{

struct AppletContainerPrivate
{
    AppletContainer* q;
    QString m_appletName;
    bool m_firstShow;

    void createApplet()
    {
        QProcess* process = new QProcess(q);
        QString binPath = INSTALL_PREFIX "/" APPLET_INSTALL_DIR "/" + m_appletName;
        QStringList arguments;
        arguments << QString::number(q->winId());
        process->start(binPath, arguments, QIODevice::NotOpen);
    }
};

AppletContainer::AppletContainer(const QString& appletName)
: d(new AppletContainerPrivate)
{
    d->q = this;
    d->m_appletName = appletName;
    d->m_firstShow = true;
}

AppletContainer::~AppletContainer()
{
    delete d;
}

void AppletContainer::showEvent(QShowEvent* event)
{
    QX11EmbedContainer::showEvent(event);
    if (d->m_firstShow) {
        d->createApplet();
        d->m_firstShow = false;
    }
}

bool AppletContainer::x11Event(XEvent* event)
{
    if (event->type == ConfigureNotify) {
        // The applet resized itself, adjust our size
        XSizeHints hints;
        long supplied;

        if (XGetWMNormalHints(QX11Info::display(), clientWinId(), &hints, &supplied)) {
            QSize size;
            if (hints.flags & PMinSize) {
                size = QSize(qMax(hints.min_width, 1), qMax(hints.min_height, 1));
            } else if (hints.flags & PBaseSize) {
                size = QSize(qMax(hints.base_width, 1), qMax(hints.base_height, 1));
            }
            setFixedSize(size);
        }
    }
    return QX11EmbedContainer::x11Event(event);
}

} // namespace

#include "appletcontainer.moc"
