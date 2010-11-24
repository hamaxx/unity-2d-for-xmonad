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
#include <QPushButton>

namespace UnityQt
{

typedef QPushButton WindowButton;

struct AppNameAppletPrivate
{
    AppNameApplet* q;
    QWidget* m_windowButtonWidget;
    WindowButton* m_closeButton;
    WindowButton* m_minimizeButton;
    WindowButton* m_maximizeButton;
    QLabel* m_label;
    WindowHelper* m_windowHelper;

    void setupLabel()
    {
        m_label = new QLabel;
        QFont font = m_label->font();
        font.setBold(true);
        m_label->setFont(font);
    }

    void setupWindowButtonWidget()
    {
        m_windowButtonWidget = new QWidget;
        QHBoxLayout* layout = new QHBoxLayout(m_windowButtonWidget);
        layout->setMargin(0);
        layout->setSpacing(0);
        m_closeButton = new WindowButton("x");
        m_minimizeButton = new WindowButton("-");
        m_maximizeButton = new WindowButton("[]");
        layout->addWidget(m_closeButton);
        layout->addWidget(m_minimizeButton);
        layout->addWidget(m_maximizeButton);
    }

    void setupWatcher()
    {
        m_windowHelper = new WindowHelper(q);
        QObject::connect(m_windowHelper, SIGNAL(stateChanged()),
            q, SLOT(updateWidgets()));
    }
};

AppNameApplet::AppNameApplet()
: d(new AppNameAppletPrivate)
{
    d->q = this;

    d->setupWatcher();
    d->setupLabel();
    d->setupWindowButtonWidget();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->m_windowButtonWidget);
    layout->addWidget(d->m_label);

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveApplicationChanged(BamfApplication*, BamfApplication*)), SLOT(updateLabel()));
    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*,BamfWindow*)), SLOT(updateWindowHelper()));
    updateLabel();
    updateWindowHelper();
    updateWidgets();
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

void AppNameApplet::updateWidgets()
{
    bool isMaximized = d->m_windowHelper->isMaximized();
    d->m_windowButtonWidget->setVisible(isMaximized);
    d->m_label->setVisible(!isMaximized);
}

} // namespace

#include "appnameapplet.moc"
