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
#include "appnameapplet.h"

// Local
#include "croppedlabel.h"
#include "menubarwidget.h"
#include "panelstyle.h"
#include "windowhelper.h"

// Unity-2d
#include <debug_p.h>
#include <keyboardmodifiersmonitor.h>
#include <launcherclient.h>

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Qt
#include <QAbstractButton>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QMenuBar>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

static const int APPNAME_LABEL_LEFT_MARGIN = 6;

namespace Unity2d
{

class WindowButton : public QAbstractButton
{
public:
    WindowButton(const PanelStyle::WindowButtonType& buttonType, QWidget* parent = 0)
    : QAbstractButton(parent)
    , m_buttonType(buttonType)
    {
        loadPixmaps();
        if (buttonType == PanelStyle::MinimizeWindowButton) {
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        } else {
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        }
        setAttribute(Qt::WA_Hover);
    }

    QSize minimumSizeHint() const
    {
        return m_normalPix.size();
    }

protected:
    bool event(QEvent* ev)
    {
        if (ev->type() == QEvent::PaletteChange) {
            loadPixmaps();
        }
        return QAbstractButton::event(ev);
    }

    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        QPixmap pix;
        if (isDown()) {
            pix = m_downPix;
        } else if (underMouse()) {
            pix = m_hoverPix;
        } else {
            pix = m_normalPix;
        }
        int posX;
        if (m_buttonType == PanelStyle::CloseWindowButton) {
            posX = width() - pix.width();
        } else {
            posX = 0;
        }
        painter.drawPixmap(posX, (height() - pix.height()) / 2, pix);
    }

private:
    PanelStyle::WindowButtonType m_buttonType;
    QPixmap m_normalPix;
    QPixmap m_hoverPix;
    QPixmap m_downPix;

    void loadPixmaps()
    {
        PanelStyle* style = PanelStyle::instance();
        m_normalPix = style->windowButtonPixmap(m_buttonType, PanelStyle::NormalState);
        m_hoverPix = style->windowButtonPixmap(m_buttonType, PanelStyle::PrelightState);
        m_downPix = style->windowButtonPixmap(m_buttonType, PanelStyle::PressedState);
    }
};

struct AppNameAppletPrivate
{
    AppNameApplet* q;
    QWidget* m_windowButtonWidget;
    WindowButton* m_closeButton;
    WindowButton* m_minimizeButton;
    WindowButton* m_maximizeButton;
    QLabel* m_label;
    WindowHelper* m_windowHelper;
    MenuBarWidget* m_menuBarWidget;

    void setupLabel()
    {
        m_label = new CroppedLabel;
        m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_label->setTextFormat(Qt::PlainText);
        // Align left of label with left of menubar
        m_label->setContentsMargins(APPNAME_LABEL_LEFT_MARGIN, 0, 0, 0);
        QFont font = m_label->font();
        font.setBold(true);
        m_label->setFont(font);
    }

    void setupWindowButtonWidget()
    {
        m_windowButtonWidget = new QWidget;
        m_windowButtonWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        QHBoxLayout* layout = new QHBoxLayout(m_windowButtonWidget);
        layout->setMargin(0);
        layout->setSpacing(0);
        m_closeButton = new WindowButton(PanelStyle::CloseWindowButton);
        m_minimizeButton = new WindowButton(PanelStyle::MinimizeWindowButton);
        m_maximizeButton = new WindowButton(PanelStyle::UnmaximizeWindowButton);
        layout->addWidget(m_closeButton);
        layout->addWidget(m_minimizeButton);
        layout->addWidget(m_maximizeButton);
        m_windowButtonWidget->setFixedWidth(LauncherClient::MaximumWidth);
        QObject::connect(m_closeButton, SIGNAL(clicked()), m_windowHelper, SLOT(close()));
        QObject::connect(m_minimizeButton, SIGNAL(clicked()), m_windowHelper, SLOT(minimize()));
        QObject::connect(m_maximizeButton, SIGNAL(clicked()), m_windowHelper, SLOT(unmaximize()));
    }

    void setupWindowHelper()
    {
        m_windowHelper = new WindowHelper(q);
        QObject::connect(m_windowHelper, SIGNAL(stateChanged()),
            q, SLOT(updateWidgets()));
        QObject::connect(m_windowHelper, SIGNAL(nameChanged()),
            q, SLOT(updateWidgets()));
    }

    void setupMenuBarWidget(IndicatorsManager* manager)
    {
        m_menuBarWidget = new MenuBarWidget(manager);
        QObject::connect(m_menuBarWidget, SIGNAL(isOpenedChanged()),
            q, SLOT(updateWidgets()));
        QObject::connect(m_menuBarWidget, SIGNAL(isEmptyChanged()),
            q, SLOT(updateWidgets()));
    }

    void setupKeyboardModifiersMonitor()
    {
        QObject::connect(KeyboardModifiersMonitor::instance(), SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
            q, SLOT(updateWidgets()));
    }
};

AppNameApplet::AppNameApplet(IndicatorsManager* indicatorsManager)
: d(new AppNameAppletPrivate)
{
    d->q = this;
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    d->setupWindowHelper();
    d->setupLabel();
    d->setupWindowButtonWidget();
    d->setupMenuBarWidget(indicatorsManager);
    d->setupKeyboardModifiersMonitor();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d->m_windowButtonWidget);
    layout->addWidget(d->m_label);
    layout->addWidget(d->m_menuBarWidget);

    updateWidgets();
}

AppNameApplet::~AppNameApplet()
{
    delete d;
}

void AppNameApplet::updateWidgets()
{
    BamfApplication* app = BamfMatcher::get_default().active_application();

    bool isMaximized = d->m_windowHelper->isMaximized();
    bool isUserVisibleApp = app ? app->user_visible() : false;
    bool isOnSameScreen = d->m_windowHelper->isMostlyOnScreen(QApplication::desktop()->screenNumber(this));
    bool isUnderMouse = rect().contains(mapFromGlobal(QCursor::pos()));
    bool showMenu = (!d->m_menuBarWidget->isEmpty() && isUserVisibleApp && isOnSameScreen)
        && (isUnderMouse
        || KeyboardModifiersMonitor::instance()->keyboardModifiers() == Qt::AltModifier
        || d->m_menuBarWidget->isOpened()
        );
    bool showLabel = !(isMaximized && showMenu) && isUserVisibleApp && isOnSameScreen;

    d->m_windowButtonWidget->setVisible(isOnSameScreen && isMaximized && isUnderMouse);

    d->m_label->setVisible(showLabel);
    if (showLabel) {
        // Define text
        QString text;
        if (app) {
            if (isMaximized) {
                // When maximized, show window title
                BamfWindow* bamfWindow = BamfMatcher::get_default().active_window();
                if (bamfWindow) {
                    text = bamfWindow->name();
                }
            } else {
                // When not maximized, show application name
                text = app->name();
            }
        }
        d->m_label->setText(text);

        // Define width
        if (!isMaximized && showMenu) {
            d->m_label->setMaximumWidth(LauncherClient::MaximumWidth);
        } else {
            d->m_label->setMaximumWidth(QWIDGETSIZE_MAX);
        }
    }

    d->m_menuBarWidget->setVisible(showMenu);
}

void AppNameApplet::enterEvent(QEvent*) {
    updateWidgets();
}

void AppNameApplet::leaveEvent(QEvent*) {
    updateWidgets();
}

} // namespace

#include "appnameapplet.moc"
