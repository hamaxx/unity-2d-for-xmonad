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
#include "menubarwidget.h"
#include "windowhelper.h"

// Unity-qt
#include <debug_p.h>
#include <keyboardmodifiersmonitor.h>

// Bamf
#include <bamf-application.h>
#include <bamf-matcher.h>

// Qt
#include <QAbstractButton>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPainter>

static const char* METACITY_DIR = "/usr/share/themes/Ambiance/metacity-1";

static const int WINDOW_BUTTONS_RIGHT_MARGIN = 4;

static const int APPNAME_LABEL_LEFT_MARGIN = 12;

namespace UnityQt
{

class WindowButton : public QAbstractButton
{
public:
    WindowButton(const QString& prefix, QWidget* parent = 0)
    : QAbstractButton(parent)
    , m_prefix(prefix)
    , m_normalPix(loadPix("normal"))
    , m_hoverPix(loadPix("prelight"))
    , m_downPix(loadPix("pressed"))
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        setAttribute(Qt::WA_Hover);
    }

    QSize minimumSizeHint() const
    {
        return m_normalPix.size();
    }

protected:
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
        painter.drawPixmap((width() - pix.width()) / 2, (height() - pix.height()) / 2, pix);
    }

private:
    QString m_prefix;
    QPixmap m_normalPix;
    QPixmap m_hoverPix;
    QPixmap m_downPix;

    QPixmap loadPix(const QString& name)
    {
        QString path = QString("%1/%2_focused_%3.png")
            .arg(METACITY_DIR)
            .arg(m_prefix)
            .arg(name);
        return QPixmap(path);
    }
};

/**
 * This label makes sure minimumSizeHint() is not set. This ensures the applet
 * does not get wider if a window title is very long
 */
class CroppedLabel : public QLabel
{
public:
    CroppedLabel(QWidget* parent = 0)
    : QLabel(parent)
    {}

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint();
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
        QHBoxLayout* layout = new QHBoxLayout(m_windowButtonWidget);
        layout->setContentsMargins(0, 0, WINDOW_BUTTONS_RIGHT_MARGIN, 0);
        layout->setSpacing(0);
        m_closeButton = new WindowButton("close");
        m_minimizeButton = new WindowButton("minimize");
        m_maximizeButton = new WindowButton("unmaximize");
        layout->addWidget(m_closeButton);
        layout->addWidget(m_minimizeButton);
        layout->addWidget(m_maximizeButton);
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

    void setupMenuBarWidget()
    {
        m_menuBarWidget = new MenuBarWidget(0 /* Window menu */);
        QObject::connect(m_menuBarWidget, SIGNAL(menuBarClosed()),
            q, SLOT(updateWidgets()));
    }

    void setupKeyboardModifiersMonitor()
    {
        QObject::connect(KeyboardModifiersMonitor::instance(), SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
            q, SLOT(updateWidgets()));
    }
};

AppNameApplet::AppNameApplet()
: d(new AppNameAppletPrivate)
{
    d->q = this;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    d->setupWindowHelper();
    d->setupLabel();
    d->setupWindowButtonWidget();
    d->setupMenuBarWidget();
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
    bool isMaximized = d->m_windowHelper->isMaximized();
    bool menuBarIsEmpty = d->m_menuBarWidget->menuBar()->actions().isEmpty();
    bool menuBarIsOpened = d->m_menuBarWidget->menuBar()->activeAction();
    bool showMenu = !menuBarIsEmpty
        && (window()->underMouse()
        || KeyboardModifiersMonitor::instance()->keyboardModifiers() == Qt::AltModifier
        || menuBarIsOpened);

    d->m_windowButtonWidget->setVisible(isMaximized);

    bool showLabel = !(isMaximized && showMenu);
    d->m_label->setVisible(showLabel);
    if (showLabel) {
        // Define text
        QString text;
        if (isMaximized) {
            // When maximized, show window title
            BamfWindow* bamfWindow = BamfMatcher::get_default().active_window();
            if (bamfWindow) {
                text = bamfWindow->name();
            }
        } else {
            // When not maximized, show application name
            BamfApplication* app = BamfMatcher::get_default().active_application();
            if (app) {
                text = app->name();
            }
        }
        d->m_label->setText(text);

        // Define width
        if (!isMaximized && showMenu) {
            d->m_label->setMaximumWidth(d->m_windowButtonWidget->sizeHint().width());
        } else {
            d->m_label->setMaximumWidth(QWIDGETSIZE_MAX);
        }
    }

    d->m_menuBarWidget->setVisible(showMenu);
}

bool AppNameApplet::event(QEvent* event)
{
    if (event->type() == QEvent::ParentChange) {
        // Install an event filter on the panel to detect mouse over
        window()->installEventFilter(this);
    }
    return Applet::event(event);
}

bool AppNameApplet::eventFilter(QObject*, QEvent* event)
{
    switch (event->type()) {
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        updateWidgets();
        break;
    default:
        break;
    }
    return false;
}

} // namespace

#include "appnameapplet.moc"
