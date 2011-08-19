/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Olivier Tilloy <olivier.tilloy@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

// Self
#include "panelmanager.h"

// Local
#include <config.h>

// Unity
#include <unity2dpanel.h>
#include <panelappletproviderinterface.h>

// QConf
#include <qconf.h>

// Qt
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QPluginLoader>
#include <QProcessEnvironment>
#include <QVariant>

using namespace Unity2d;

static const char* PANEL_DCONF_SCHEMA = "com.canonical.Unity2d.Panel";
static const char* PANEL_DCONF_PROPERTY_APPLETS = "applets";
static const char* PANEL_PLUGINS_DEV_DIR_ENV = "UNITY2D_PANEL_PLUGINS_PATH";

static QPalette getPalette()
{
    QPalette palette;

    /* Should use the panel's background provided by Unity but it turns
       out not to be good. It would look like:

         QBrush bg(QPixmap("theme:/panel_background.png"));
    */
    QBrush bg(QPixmap(unity2dDirectory() + "/panel/artwork/background.png"));
    palette.setBrush(QPalette::Window, bg);
    palette.setBrush(QPalette::Button, bg);
    return palette;
}

static QHash<QString, PanelAppletProviderInterface*> loadPlugins()
{
    QHash<QString, PanelAppletProviderInterface*> plugins;

    /* When running uninstalled the plugins will be loaded from the source tree
       under panel/applets.
       When running installed the plugins will be typically in /usr/lib/unity-2d/plugins/panel
       In both cases it's still possible to override these default locations by setting an
       environment variable with the path where the plugins are located.
    */

    QString panelPluginsDefaultDir = unity2dPluginsPath() + "/panel";
    if (!isRunningInstalled()) panelPluginsDefaultDir += "/applets";
    QString pluginPath = QProcessEnvironment::systemEnvironment().value(PANEL_PLUGINS_DEV_DIR_ENV,
                                                                        panelPluginsDefaultDir);
    QFileInfo pluginFileInfo = QFileInfo(pluginPath);
    if (!pluginFileInfo.isDir()) {
        qWarning() << "Panel plugin directory does not exist:" << pluginPath;
        return plugins;
    }

    qDebug() << "Scanning panel plugin directory" << pluginFileInfo.absoluteFilePath();

    QDir pluginDir = QDir(pluginFileInfo.absoluteFilePath());
    QStringList filters;
    filters << "*.so";
    pluginDir.setNameFilters(filters);

    Q_FOREACH(QString fileEntry, pluginDir.entryList()) {
        QString pluginFilePath = pluginDir.absoluteFilePath(fileEntry);
        qDebug() << "Loading panel plugin:" << pluginFilePath;

        QPluginLoader loader(pluginFilePath);
        if (loader.load()) {
            PanelAppletProviderInterface* provider;
            provider = qobject_cast<PanelAppletProviderInterface*>(loader.instance());
            if (provider == 0) {
                qWarning() << "Plugin loaded from" << pluginFilePath
                           << "does not implement the interface PanelAppletProviderInterface";
            } else {
               plugins.insert(provider->appletName(), provider);
            }
        } else {
            qWarning() << "Failed to load panel plugin from" << pluginFilePath
                       << "with error" << loader.errorString();
        }
    }

    return plugins;
}

static QStringList loadPanelConfiguration()
{
    QConf panelConfig(PANEL_DCONF_SCHEMA);

    QVariant appletsConfig = panelConfig.property(PANEL_DCONF_PROPERTY_APPLETS);
    if (!appletsConfig.isValid()) {
        qWarning() << "Missing or invalid panel applets configuration in dconf. Please check"
                   << "the property" << PANEL_DCONF_PROPERTY_APPLETS
                   << "in schema" << PANEL_DCONF_SCHEMA;
        return QStringList();
    }

    return appletsConfig.toStringList();
}

static Unity2dPanel* instantiatePanel(int screen)
{
    Unity2dPanel* panel = new Unity2dPanel;
    panel->setEdge(Unity2dPanel::TopEdge);
    panel->setPalette(getPalette());
    panel->setFixedHeight(24);

    int leftmost = QApplication::desktop()->screenNumber(QPoint());

    QHash<QString, PanelAppletProviderInterface*> plugins = loadPlugins();

    QStringList panelConfiguration = loadPanelConfiguration();
    qDebug() << "Configured plugins list is:" << panelConfiguration;

    Q_FOREACH(QString appletName, panelConfiguration) {
        bool onlyLeftmost = appletName.startsWith('!');
        if (onlyLeftmost) {
            appletName = appletName.mid(1);
        }

        PanelAppletProviderInterface* provider = plugins.value(appletName, NULL);
        if (provider == 0) {
            qWarning() << "Panel applet" << appletName << "was requested but there's no"
                       << "installed plugin providing it.";
        } else {
            if (screen == leftmost || !onlyLeftmost) {
                QWidget *applet = provider->createApplet();
                if (applet == 0) {
                    qWarning() << "The panel applet plugin for" << appletName
                               << "did not return a valid plugin.";
                } else {
                    panel->addWidget(applet);
                }
            }
        }
    }

    return panel;
}

PanelManager::PanelManager(QObject* parent)
    : QObject(parent)
{
    QDesktopWidget* desktop = QApplication::desktop();
    for(int i = 0; i < desktop->screenCount(); ++i) {
        Unity2dPanel* panel = instantiatePanel(i);
        m_panels.append(panel);
        panel->show();
        panel->move(desktop->screenGeometry(i).topLeft());
    }
    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));
}

PanelManager::~PanelManager()
{
    qDeleteAll(m_panels);
}

void
PanelManager::onScreenCountChanged(int newCount)
{
    if (newCount > 0) {
        QDesktopWidget* desktop = QApplication::desktop();
        int size = m_panels.size();
        Unity2dPanel* panel;

        /* The first panel is always the one on the leftmost screen. */
        int leftmost = desktop->screenNumber(QPoint());
        if (size > 0) {
            panel = m_panels[0];
        } else {
            panel = instantiatePanel(leftmost);
            m_panels.append(panel);
        }
        panel->show();
        panel->move(desktop->screenGeometry(leftmost).topLeft());

        /* Update the position of other existing panels, and instantiate new
           panels as needed. */
        int i = 1;
        for (int screen = 0; screen < newCount; ++screen) {
            if (screen == leftmost) {
                continue;
            }
            if (i < size) {
                panel = m_panels[i];
            } else {
                panel = instantiatePanel(screen);
                m_panels.append(panel);
            }
            panel->show();
            panel->move(desktop->screenGeometry(screen).topLeft());
            ++i;
        }
    }
    /* Remove extra panels if any. */
    while (m_panels.size() > newCount) {
        delete m_panels.takeLast();
    }
}

#include "panelmanager.moc"

