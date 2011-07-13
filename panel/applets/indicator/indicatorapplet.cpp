/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

// Self
#include "indicatorapplet.h"

// Local
#include "abstractindicator.h"
#include "datetimeindicator.h"
#include "debug_p.h"
#include "indicator.h"

// Qt
#include <QAction>
#include <QDBusConnection>
#include <QHBoxLayout>
#include <QMenu>
#include <QX11EmbedContainer>

// Gtk
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

IndicatorApplet::IndicatorApplet()
{
    setupUi();
    loadIndicators();
}

void IndicatorApplet::setupUi()
{
    m_menuBar = new QMenuBar;
    m_menuBar->setNativeMenuBar(false);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_menuBar);

    QMetaObject::invokeMethod(this, "createGtkIndicator", Qt::QueuedConnection);
}

void IndicatorApplet::createGtkIndicator()
{
    m_container = new QX11EmbedContainer;
    layout()->addWidget(m_container);

    m_gtkIndicator = indicator_new();
    m_container->embedClient(gtk_plug_get_id(GTK_PLUG(m_gtkIndicator->container)));
    gtk_widget_show(m_gtkIndicator->container);

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), SLOT(adjustGtkIndicatorSize()));
    timer->start();
}

void IndicatorApplet::adjustGtkIndicatorSize()
{
    GtkRequisition minimum, natural;
    gtk_widget_get_preferred_size(m_gtkIndicator->menu, &minimum, &natural);
    m_container->setFixedWidth(minimum.width);
}

void IndicatorApplet::loadIndicators()
{
#if 0
    // FIXME: Using Qt plugins
    QList<AbstractIndicator*> indicators = QList<AbstractIndicator*>()
        << new DateTimeIndicator(this)
        ;

    Q_FOREACH(AbstractIndicator* indicator, indicators) {
        connect(indicator, SIGNAL(actionAdded(QAction*)), SLOT(slotActionAdded(QAction*)));
        connect(indicator, SIGNAL(actionRemoved(QAction*)), SLOT(slotActionRemoved(QAction*)));
        indicator->init();
    }
#endif
}

void IndicatorApplet::slotActionAdded(QAction* action)
{
    UQ_VAR(action->text());
    m_menuBar->addAction(action);
}

void IndicatorApplet::slotActionRemoved(QAction* action)
{
    m_menuBar->removeAction(action);
}


#include "indicatorapplet.moc"
