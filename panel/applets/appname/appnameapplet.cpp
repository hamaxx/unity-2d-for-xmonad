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
    QLabel* m_label;
};

AppNameApplet::AppNameApplet()
: d(new AppNameAppletPrivate)
{
    d->m_label = new QLabel;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->m_label);

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveApplicationChanged(BamfApplication*, BamfApplication*)), SLOT(updateLabel()));
    updateLabel();
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
    adjustSize();
}

} // namespace

APPLET_MAIN(UnityQt::AppNameApplet)

#include "appnameapplet.moc"
