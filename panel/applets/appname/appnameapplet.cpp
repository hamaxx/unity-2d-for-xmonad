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
#include "appnameapplet.h"

// Local
#include "windowhelper.h"

// Unity-qt
#include <debug_p.h>

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Qt
#include <QHBoxLayout>
#include <QLabel>

namespace UnityQt
{

struct AppNameAppletPrivate
{
    AppNameApplet* q;
    QLabel* m_label;
    WindowHelper* m_windowHelper;

    void setupWatcher()
    {
        m_windowHelper = new WindowHelper(q);
        QObject::connect(m_windowHelper, SIGNAL(stateChanged()),
            q, SLOT(updateWindowButtonWidget()));
    }
};

AppNameApplet::AppNameApplet()
: d(new AppNameAppletPrivate)
{
    d->q = this;
    d->m_label = new QLabel;
    QFont font = d->m_label->font();
    font.setBold(true);
    d->m_label->setFont(font);

    d->setupWatcher();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->m_label);

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveApplicationChanged(BamfApplication*, BamfApplication*)), SLOT(updateLabel()));
    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*,BamfWindow*)), SLOT(updateWindowHelper()));
    updateLabel();
    updateWindowHelper();
    updateWindowButtonWidget();
}

AppNameApplet::~AppNameApplet()
{
    delete d;
}

void AppNameApplet::updateLabel()
{
    BamfApplication* app = BamfMatcher::get_default().active_application();
    if (app) {
        d->m_label->setText(app->name());
    } else {
        d->m_label->setText(QString());
    }
}

void AppNameApplet::updateWindowHelper()
{
    BamfWindow* window = BamfMatcher::get_default().active_window();
    d->m_windowHelper->setXid(window ? window->xid() : 0);
}

void AppNameApplet::updateWindowButtonWidget()
{
    UQ_VAR(d->m_windowHelper->isMaximized());
}

} // namespace

#include "appnameapplet.moc"
