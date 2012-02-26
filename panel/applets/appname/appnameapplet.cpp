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

/*
 * Modified by:
 * - Jure Ham <jure@hamsworld.net>
 */

// Self
#include "appnameapplet.h"

// Local
#include "config.h"
#include "croppedlabel.h"
#include "menubarwidget.h"
#include "panelstyle.h"
#include "unity2dpanel.h"
#include "windowhelper.h"
#include "dashclient.h"
#include "hudclient.h"

// Unity-2d
#include <debug_p.h>
#include <keymonitor.h>
#include <launcherclient.h>
#include <hotkey.h>
#include <hotkeymonitor.h>
#include <indicatorentrywidget.h>
#include <unity2dtr.h>

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Qt
#include <QAbstractButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QMenuBar>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPoint>

static const char* PANEL_DCONF_SCHEMA = "com.canonical.Unity2d.Panel";
static const char* PANEL_DCONF_PROPERTY_XMONADLOG = "xmonadlog";

static const int APPNAME_LABEL_LEFT_MARGIN = 6;

class WindowButton : public QAbstractButton
{
public:
    WindowButton(const PanelStyle::WindowButtonType& buttonType, QWidget* parent = 0)
    : QAbstractButton(parent)
    , m_isDashButton(false)
    , m_initialized(false)
    {
        setButtonType(buttonType);
        if (buttonType == PanelStyle::MinimizeWindowButton) {
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        } else {
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        }
        setAttribute(Qt::WA_Hover);
        m_initialized = true;
    }

    void setButtonType(const PanelStyle::WindowButtonType& buttonType)
    {
        if (m_initialized && m_buttonType == buttonType) {
            return;
        }

        m_buttonType = buttonType;
        loadPixmaps(false);
        update();
    }

    QSize minimumSizeHint() const
    {
        return m_normalPix.size();
    }

    void setIsDashButton(bool isDashButton)
    {
        if (m_initialized && m_isDashButton == isDashButton) {
            return;
        }

        m_isDashButton = isDashButton;
        update();
    }

protected:
    bool event(QEvent* ev)
    {
        if (ev->type() == QEvent::PaletteChange) {
            loadPixmaps(true);
        }
        return QAbstractButton::event(ev);
    }

    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        QPixmap pix;
        if (isEnabled()) {
            if (isDown()) {
                pix = (m_isDashButton) ? m_dash_downPix : m_downPix;
            } else if (underMouse()) {
                pix = (m_isDashButton) ? m_dash_hoverPix : m_hoverPix;
            } else {
                pix = (m_isDashButton) ? m_dash_normalPix : m_normalPix;
            }
        } else {
            if (m_buttonType == PanelStyle::MaximizeWindowButton && m_isDashButton) {
                /* we have disabled asset only for dash maximize button */
                pix = m_dash_disabledPix;
            } else if (m_isDashButton) {
                pix = m_dash_normalPix;
            } else {
                pix = m_normalPix;
            }
        }
        bool rtl = layoutDirection() == Qt::RightToLeft;
        int posX;
        if ((!rtl && m_buttonType == PanelStyle::CloseWindowButton) ||
            ( rtl && m_buttonType != PanelStyle::CloseWindowButton)) {
            posX = width() - pix.width();
        } else {
            posX = 0;
        }
        painter.drawPixmap(posX, (height() - pix.height()) / 2, pix);
    }

private:
    PanelStyle::WindowButtonType m_buttonType;
    bool m_isDashButton;
    QPixmap m_normalPix;
    QPixmap m_hoverPix;
    QPixmap m_downPix;
    QPixmap m_dash_normalPix;
    QPixmap m_dash_hoverPix;
    QPixmap m_dash_downPix;
    QPixmap m_dash_disabledPix;
    bool m_initialized;

    void loadPixmaps(bool loadOnlyStylePixmaps)
    {
        PanelStyle* style = PanelStyle::instance();
        m_normalPix = style->windowButtonPixmap(m_buttonType, PanelStyle::NormalState);
        m_hoverPix = style->windowButtonPixmap(m_buttonType, PanelStyle::PrelightState);
        m_downPix = style->windowButtonPixmap(m_buttonType, PanelStyle::PressedState);

        if (!loadOnlyStylePixmaps) {
            loadDashPixmaps(m_buttonType);
        }
    }

    void loadDashPixmaps(PanelStyle::WindowButtonType buttonType)
    {
        QString iconPath = unity2dDirectory() + "/panel/applets/appname/artwork/";

        switch (buttonType) {
        case PanelStyle::CloseWindowButton:
            iconPath += "close_dash";
            break;
        case PanelStyle::MinimizeWindowButton:
            iconPath += "minimize_dash";
            break;
        case PanelStyle::UnmaximizeWindowButton:
            iconPath += "unmaximize_dash";
            break;
        case PanelStyle::MaximizeWindowButton:
            iconPath += "maximize_dash";
            /* we have disabled asset only for maximize button */
            m_dash_disabledPix.load(iconPath + "_disabled.png");
            break;
        }

        m_dash_normalPix.load(iconPath + ".png");
        m_dash_hoverPix.load(iconPath + "_prelight.png");
        m_dash_downPix.load(iconPath + "_pressed.png");
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
    QPoint m_dragStartPosition;
    bool m_dragInProgress;

    AppNameAppletPrivate()
    : m_dragInProgress(false)
    {}

    void setupLabel()
    {
        m_label = new CroppedLabel;
        m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        //m_label->setTextFormat(Qt::PlainText);
        // Align left of label with left of menubar
        if (QApplication::isLeftToRight()) {
            m_label->setContentsMargins(APPNAME_LABEL_LEFT_MARGIN, 0, 0, 0);
        } else {
            m_label->setContentsMargins(0, 0, APPNAME_LABEL_LEFT_MARGIN, 0);
        }
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
        m_closeButton->setObjectName("AppNameApplet::CloseButton");
        m_minimizeButton = new WindowButton(PanelStyle::MinimizeWindowButton);
        m_minimizeButton->setObjectName("AppNameApplet::MinimizeButton");
        m_maximizeButton = new WindowButton(PanelStyle::UnmaximizeWindowButton);
        m_maximizeButton->setObjectName("AppNameApplet::MaximizeButton");
        layout->addWidget(m_closeButton);
        layout->addWidget(m_minimizeButton);
        layout->addWidget(m_maximizeButton);
        m_windowButtonWidget->setFixedWidth(LauncherClient::MaximumWidth);
        QObject::connect(m_closeButton, SIGNAL(clicked()), m_windowHelper, SLOT(close()));
        QObject::connect(m_minimizeButton, SIGNAL(clicked()), m_windowHelper, SLOT(minimize()));
        QObject::connect(m_maximizeButton, SIGNAL(clicked()), m_windowHelper, SLOT(toggleMaximize()));
    }

    void setupWindowHelper()
    {
        m_windowHelper = new WindowHelper(q->panel(), q);
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
        QObject::connect(KeyMonitor::instance(), SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
            q, SLOT(updateWidgets()));
    }
};

AppNameApplet::AppNameApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
, d(new AppNameAppletPrivate)
, m_conf(new QConf(PANEL_DCONF_SCHEMA))
{
    d->q = this;
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    d->setupWindowHelper();
    d->setupLabel();
    d->setupWindowButtonWidget();
    d->setupMenuBarWidget(panel->indicatorsManager());
    d->setupKeyboardModifiersMonitor();

    connect(DashClient::instance(), SIGNAL(alwaysFullScreenChanged()), SLOT(updateWidgets()));
    connect(DashClient::instance(), SIGNAL(dashDisconnected()), SLOT(updateWidgets()));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d->m_windowButtonWidget);
    layout->addWidget(d->m_label);
    layout->addWidget(d->m_menuBarWidget);

    if (panel != NULL) {
        panel->installEventFilter(this);
    }

    displayXmonadLog = m_conf->property(PANEL_DCONF_PROPERTY_XMONADLOG).toBool();

    xmonadLog = QString("");
    if (displayXmonadLog) {
        QDBusConnection bus = QDBusConnection::sessionBus();
        bus.connect("", "", "org.xmonad.Log", "Update", this, SLOT(logReceived(const QDBusMessage &)));
    }
    updateWidgets();
}

AppNameApplet::~AppNameApplet()
{
    delete d;
}

void AppNameApplet::logReceived(const QDBusMessage &msg)
{
    xmonadLog = msg.arguments().at(0).toString();
}

void AppNameApplet::updateWidgets()
{
    BamfApplication* app = BamfMatcher::get_default().active_application();

    bool isMaximized = d->m_windowHelper->isMaximized();
    bool isUserVisibleApp = app ? app->user_visible() : false;
    bool isOnSameScreen = d->m_windowHelper->isMostlyOnScreen(panel()->screen());
    bool isUnderMouse = rect().contains(mapFromGlobal(QCursor::pos()));
    bool isOpened = isOnSameScreen &&
        (isUnderMouse
        || KeyMonitor::instance()->keyboardModifiers() == Qt::AltModifier
        || d->m_menuBarWidget->isOpened()
        );
    bool showDesktopLabel = !app;
    bool showMenu = isOpened && !d->m_menuBarWidget->isEmpty() && (isUserVisibleApp || showDesktopLabel);
    bool dashCanResize = !DashClient::instance()->alwaysFullScreen();
    bool dashIsVisible = DashClient::instance()->activeInScreen(panel()->screen());
    bool hudIsVisible = HUDClient::instance()->activeInScreen(panel()->screen());
    bool showWindowButtons = (isOpened && isMaximized) || dashIsVisible || hudIsVisible;
    bool showAppLabel = !(isMaximized && showMenu) && isUserVisibleApp && isOnSameScreen;

    bool showLabel = !(isMaximized && showMenu) && isUserVisibleApp; //show label for applications on all screens

    d->m_windowButtonWidget->setVisible(showWindowButtons);
    d->m_maximizeButton->setIsDashButton(dashIsVisible);
    d->m_maximizeButton->setButtonType(isMaximized ?
                                       PanelStyle::UnmaximizeWindowButton :
                                       PanelStyle::MaximizeWindowButton);
    /* disable the minimize button for the dash & hud */
    d->m_minimizeButton->setEnabled(!dashIsVisible || !hudIsVisible);
    d->m_minimizeButton->setIsDashButton(dashIsVisible || hudIsVisible);
    /* disable the maximize button for the HUD, and when the dash is not resizeable */
    d->m_maximizeButton->setEnabled((dashIsVisible && dashCanResize) || !hudIsVisible);
    d->m_maximizeButton->setIsDashButton(dashIsVisible || hudIsVisible);
    /* make sure we use the right button for dash */
    d->m_closeButton->setIsDashButton(dashIsVisible || hudIsVisible);

    if (showAppLabel || showDesktopLabel || dashIsVisible || hudIsVisible) {
        d->m_label->setVisible(true);
        if (showAppLabel) {
            // Define text
            QString text;
            if (app) {
                if (isMaximized) {
                    // When maximized, show window title
                    BamfWindow* bamfWindow = BamfMatcher::get_default().active_window();
		    if (bamfWindow) {
		      if (displayXmonadLog && !xmonadLog.isEmpty()) {
			text = bamfWindow->name();
			text.sprintf("%s | <span>%s</span> : <span>%s</span>",
				     xmonadLog.toUtf8().constData(),
				     app->name().toUtf8().constData(),
				     bamfWindow->name().toUtf8().constData());
		      } else {
			text.sprintf("<span>%s</span> : <span>%s</span>",
				     app->name().toUtf8().constData(),
				     bamfWindow->name().toUtf8().constData());
			// When not maximized, show application name
			text = app->name();
		      }
		      d->m_label->setVisible(showLabel);
		    }
		}
	    }
	    d->m_label->setText(text);
	} else if (showDesktopLabel) {
	      d->m_label->setText(u2dTr("Ubuntu Desktop"));
        } else {
            d->m_label->setText(QString());
        }

        // Define label width
        if (!isMaximized && showMenu) {
            d->m_label->setMaximumWidth(LauncherClient::MaximumWidth);
        } else {
            d->m_label->setMaximumWidth(QWIDGETSIZE_MAX);
        }
    } else {
        d->m_label->setVisible(false);
    }

    d->m_menuBarWidget->setVisible(showMenu);
}

void AppNameApplet::enterEvent(QEvent*) {
    updateWidgets();
}

void AppNameApplet::leaveEvent(QEvent*) {
    updateWidgets();
}

void AppNameApplet::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        d->m_windowHelper->unmaximize();
    }
}

void AppNameApplet::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        d->m_dragStartPosition = event->pos();
        d->m_dragInProgress = true;
    } else {
        Unity2d::PanelApplet::mousePressEvent(event);
    }
}

void AppNameApplet::mouseReleaseEvent(QMouseEvent* event) {
    if (d->m_dragInProgress && event->button() == Qt::LeftButton) {
        d->m_dragInProgress = false;
    } else {
        Unity2d::PanelApplet::mouseReleaseEvent(event);
    }
}

void AppNameApplet::mouseMoveEvent(QMouseEvent* event) {
    if (d->m_dragInProgress && (event->buttons() & Qt::LeftButton)) {
        if ((event->pos() - d->m_dragStartPosition).manhattanLength()
                >= QApplication::startDragDistance()) {
            d->m_dragInProgress = false;
            d->m_windowHelper->drag(d->m_dragStartPosition);
        }
    } else {
        Unity2d::PanelApplet::mouseMoveEvent(event);
    }
}

bool AppNameApplet::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == Unity2dPanel::SHOW_FIRST_MENU_EVENT) {
        BamfApplication* app = BamfMatcher::get_default().active_application();
        bool isActiveAppVisible = app ? app->user_visible() : false;
        if (isActiveAppVisible) {
            d->m_menuBarWidget->setOpened(true);

            QList<IndicatorEntryWidget*> list = d->m_menuBarWidget->entries();
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

#include "appnameapplet.moc"
