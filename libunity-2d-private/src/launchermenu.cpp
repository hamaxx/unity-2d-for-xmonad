/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "launchermenu.h"
#include "launcheritem.h"

#include <QAction>
#include <QFile>
#include <QApplication>
#include <QResizeEvent>
#include <QBitmap>
#include <QX11Info>
#include <QDesktopWidget>
#include <QPainter>

LauncherContextualMenu::LauncherContextualMenu():
    QMenu(0), m_folded(true), m_launcherItem(NULL), m_titleAction(NULL)
{
    /* Timer used for to hide the menu after a given delay (hideWithDelay()) */
    m_hidingDelayTimer.setSingleShot(true);
    QObject::connect(&m_hidingDelayTimer, SIGNAL(timeout()), this, SLOT(hide()));

    /* The tooltip/menu shouldn’t be modal. */
    setWindowFlags(Qt::ToolTip);

    /* The tooltip/menu should not move when switching workspaces. */
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    /* Use transparency if available.
       Warning: if the availability of transparency changes over time, for
                example because a compositing window manager is launched, we
                do not react to that and the menu is likely to break visually.
                Ref: http://bugreports.qt.nokia.com/browse/QTBUG-6044
    */
    if (transparencyAvailable()) {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    /* Custom appearance. */
    loadCSS();

    /* Load the pixmap for the arrow. It is drawn separately as its position
       may vary depending on the position of the menu on the screen. */
    if (transparencyAvailable()) {
        if (QApplication::isLeftToRight()) {
            m_arrow.load(":/launchermenu/arrow.png");
        } else {
            m_arrow.load(":/launchermenu/arrow_rtl.png");
        }
    } else {
        if (QApplication::isLeftToRight()) {
            m_arrow.load(":/launchermenu/arrow_no_transparency.png");
        } else {
            m_arrow.load(":/launchermenu/arrow_no_transparency_rtl.png");
        }
    }

    /* First action used to display the title of the item */
    m_titleAction = new QAction(this);
    m_titleAction->setEnabled(false);
    addAction(m_titleAction);
}

LauncherContextualMenu::~LauncherContextualMenu()
{
}

void
LauncherContextualMenu::loadCSS()
{
    QString cssFilePath = ":/launchermenu/launchermenu.css";
    if (QApplication::isRightToLeft()) {
        cssFilePath = ":/launchermenu/launchermenu_rtl.css";
    }

    QFile file(cssFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString css(file.readAll());
    file.close();
    setStyleSheet(css);
}

QString
LauncherContextualMenu::title() const
{
    return m_title;
}

void
LauncherContextualMenu::setTitle(const QString& title)
{
    setAccessibleName(title);
    m_title = title;
    /* Escaping ampersands so that they are not considered as keyboard
       accelerators. */
    m_titleAction->setText(m_title.replace("&", "&&"));
    Q_EMIT titleChanged(m_title);
}

void
LauncherContextualMenu::updateMask()
{
    QPixmap pixmap(size());
    render(&pixmap, QPoint(), QRegion(),
           QWidget::DrawWindowBackground | QWidget::DrawChildren | QWidget::IgnoreMask);
    setMask(pixmap.createMaskFromColor("red"));
}

void
LauncherContextualMenu::resizeEvent(QResizeEvent* event)
{
    QMenu::resizeEvent(event);
    /* If transparent windows are not available use the XShape extension */
    if (!transparencyAvailable()) {
        updateMask();
    }
}

bool
LauncherContextualMenu::transparencyAvailable() const
{
    return QX11Info::isCompositingManagerRunning();
}

void
LauncherContextualMenu::leaveEvent(QEvent* event)
{
    /* Prepare for hiding the menu when the mouse leaves it */
    m_hidingDelayTimer.start();
    QMenu::leaveEvent(event);
}

void
LauncherContextualMenu::enterEvent(QEvent* event)
{
    /* Always keep the menu visible when the mouse hovers it */
    m_hidingDelayTimer.stop();
    QMenu::enterEvent(event);
}

void
LauncherContextualMenu::show(int x, int y)
{
    m_hidingDelayTimer.stop();

    if (isVisible())
        return;

    m_arrowY = 6;

    if (QApplication::isRightToLeft()) {
        x = QApplication::desktop()->width() - x - sizeHint().width();
    }

    move(x, y - minimumSize().height() / 2);
    QMenu::show();

    /* FIXME: adjust the position of the menu if it goes offscreen,
              as is done in setFolded(false). */
}

void
LauncherContextualMenu::hide()
{
    /* Fold the menu upon hiding */
    QMenu::hide();
    setFolded(true);
}

void
LauncherContextualMenu::hideWithDelay(int delay)
{
    m_hidingDelayTimer.setInterval(delay);
    m_hidingDelayTimer.start();
}

bool
LauncherContextualMenu::folded() const
{
    return m_folded;
}

void
LauncherContextualMenu::setFolded(int folded)
{
    if (folded == m_folded) {
        return;
    }

    if (folded) {
        /* Remove all actions but the title. */
        for (int i = actions().size(); i > 0; --i) {
            QAction* action = actions().at(i - 1);
            if (action != m_titleAction) {
                removeAction(action);
                if (action->parent() == this) {
                    /* Delete the action only if we "own" it,
                       otherwise let its parent take care of it. */
                    delete action;
                }
            }
        }
    } else {
        int prevWidth = width();
        int left = x(), top = y();
        addSeparator();
        m_launcherItem->createMenuActions();

        QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
        if (QApplication::isRightToLeft()) {
            left -= width() - prevWidth;
        }
        if (height() <= screenGeometry.height()) {
            /* Adjust the position of the menu only if it fits entirely on the screen. */
            int menuBottomEdge = y() + height();
            int screenBottomEdge = screenGeometry.y() + screenGeometry.height();
            if (menuBottomEdge > screenBottomEdge) {
                /* The menu goes offscreen, shift it upwards. */
                m_arrowY += menuBottomEdge - screenBottomEdge;
                top = screenBottomEdge - height();
            }
        }
        move(left, top);
        if (!transparencyAvailable()) {
            /* The arrow has moved relatively to the menu. */
            updateMask();
        }
    }

    m_folded = folded;

    Q_EMIT foldedChanged(m_folded);
}

void
LauncherContextualMenu::paintEvent(QPaintEvent* event)
{
    QMenu::paintEvent(event);

    /* Draw the arrow. */
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    int left = 0;
    if (QApplication::isRightToLeft()) {
        left = width() - m_arrow.width();
    }
    painter.drawPixmap(left, m_arrowY, m_arrow);
}

LauncherItem*
LauncherContextualMenu::launcherItem() const
{
    return m_launcherItem;
}

void
LauncherContextualMenu::setLauncherItem(LauncherItem* launcherItem)
{
    m_launcherItem = launcherItem;
}

void
LauncherContextualMenu::setVisible(bool value)
{
    bool changed = isVisible() != value;
    // The call to QMenu::setVisible() could probably be skipped if 'changed'
    // is false, but I'd rather keep the class behavior as close to the default
    // setVisible() implementation as possible.
    QMenu::setVisible(value);
    if (changed) {
        visibleChanged(value);
    }
}

void
LauncherContextualMenu::setFocus()
{
    /* FIXME: for some reason, invoking QMenu::activateWindow() directly here
       doesn’t work, the active window remains unchanged. */
    QTimer::singleShot(1, this, SLOT(activateWindow()));

    /* Set the first enabled action active. */
    Q_FOREACH(QAction* action, actions()) {
        if (action->isEnabled() && !action->isSeparator()) {
            setActiveAction(action);
            break;
        }
    }
}

void
LauncherContextualMenu::activateWindow()
{
    QMenu::activateWindow();
}

void
LauncherContextualMenu::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_Escape) {
        hide();
        Q_EMIT dismissedByKeyEvent();
        event->accept();
        return;
    }

    QMenu::keyPressEvent(event);
    if (event->isAccepted() && isHidden()) {
        Q_EMIT dismissedByKeyEvent();
    }
}

void
LauncherContextualMenu::insertActionBeforeTitle(QAction* action)
{
    insertAction(m_titleAction, action);
}

QAction*
LauncherContextualMenu::insertSeparatorBeforeTitle()
{
    return insertSeparator(m_titleAction);
}

#include "launchermenu.moc"
