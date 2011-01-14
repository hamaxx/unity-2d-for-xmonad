/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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
#undef signals
#include <gdk/gdk.h>
#include <gtk/gtk.h>

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
    int* argc = 0;
    char*** argv = 0;
    gtk_init(argc, argv);

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
    GtkRequisition requisition;
    gtk_widget_size_request(m_gtkIndicator->menu, &requisition);
    m_container->setFixedWidth(requisition.width);
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
