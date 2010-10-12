/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "datetimeindicator.h"

// Local
#include "indicatorservicemanager.h"

// dbusmenu-qt
#include "dbusmenuimporter.h"

// Qt
#include <QAction>
#include <QDateTime>

// From dbus-shared.h
#define  SERVICE_NAME     "org.ayatana.indicator.datetime"
#define  SERVICE_IFACE    "org.ayatana.indicator.datetime.service"
#define  SERVICE_OBJ      "/org/ayatana/indicator/datetime/service"
#define  SERVICE_VERSION  1

#define  MENU_OBJ      "/org/ayatana/indicator/datetime/menu"

#define  DBUSMENU_CALENDAR_MENUITEM_TYPE "x-canonical-calendar-item"
////

DateTimeIndicator::DateTimeIndicator(QObject* parent)
: AbstractIndicator(parent)
, m_timer(new QTimer(this))
{
    new IndicatorServiceManager(SERVICE_NAME, SERVICE_VERSION, this);

    readConfig();
    setupMenu();
    setupTimer();
    updateText();
}

void DateTimeIndicator::setupMenu()
{
    DBusMenuImporter* importer = new DBusMenuImporter(SERVICE_NAME, MENU_OBJ, this);
    action()->setMenu(importer->menu());
}

void DateTimeIndicator::setupTimer()
{
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), SLOT(updateText()));
    updateTimer();
    m_timer->start();
}

void DateTimeIndicator::updateTimer()
{
    m_timer->setInterval(1000);
}

void DateTimeIndicator::updateText()
{
    QString text = QDateTime::currentDateTime().toString(m_format);
    action()->setText(text);
}

void DateTimeIndicator::readConfig()
{
    m_format = "hh:mm:ss";
}

#include "datetimeindicator.moc"