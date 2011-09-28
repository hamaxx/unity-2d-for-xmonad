/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Marco Trevisan (Treviño) <3v1n0@ubuntu.com>
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
#include <debug_p.h>
#include <indicatorsmanager.h>
#include <indicatorswidget.h>
#include <indicatorentrywidget.h>
#include <unity2dpanel.h>

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Qt
#include <QHBoxLayout>

using namespace unity::indicator;

IndicatorApplet::IndicatorApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    m_indicatorsManager = panel->indicatorsManager();

    m_indicatorsManager->indicators()->on_object_added.connect(
        sigc::mem_fun(this, &IndicatorApplet::onObjectAdded)
        );

    m_indicatorsManager->indicators()->on_object_removed.connect(
        sigc::mem_fun(this, &IndicatorApplet::onObjectRemoved)
        );

    m_indicatorsWidget = new IndicatorsWidget(m_indicatorsManager);
    layout->addWidget(m_indicatorsWidget);

    if (panel != NULL) {
        panel->installEventFilter(this);
    }
}

bool IndicatorApplet::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == Unity2dPanel::SHOW_FIRST_MENU_EVENT) {
        BamfApplication* app = BamfMatcher::get_default().active_application();
        bool isActiveAppVisible = app ? app->user_visible() : false;
        if (!isActiveAppVisible) {
            QList<IndicatorEntryWidget*> list = m_indicatorsWidget->entries();
            if (!list.isEmpty()) {
                IndicatorEntryWidget* el = list.first();
                if (el != NULL) {
                    el->showMenu(Qt::NoButton);
                }
            }
            return true;
        } else {
            return false;
        }
    } else {
        return QWidget::eventFilter(watched, event);
    }
}

void IndicatorApplet::onObjectAdded(Indicator::Ptr const& indicator)
{
    QString name = QString::fromStdString(indicator->name());

    // appmenu indicator is handled by AppNameApplet
    if (name != "libappmenu.so") {
        m_indicatorsWidget->addIndicator(indicator);
    }
}

void IndicatorApplet::onObjectRemoved(Indicator::Ptr const& indicator)
{
    QString name = QString::fromStdString(indicator->name());

    // appmenu indicator is handled by AppNameApplet
    if (name != "libappmenu.so") {
        m_indicatorsWidget->removeIndicator(indicator);
    }
}

#include "indicatorapplet.moc"
