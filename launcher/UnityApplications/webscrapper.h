/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#ifndef WebScrapper_H
#define WebScrapper_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QStringList>

class QNetworkReply;
class LauncherApplication;

class WebScrapper : public QObject
{
    Q_OBJECT

public:
    WebScrapper(LauncherApplication* application, const QUrl& url, QObject* parent=0);
    ~WebScrapper();

    void fetchAndScrap();

private:
    LauncherApplication* m_application;
    QUrl m_url;

    QString m_title;
    QStringList m_favicons;
    QStringList::iterator m_current_favicon;
    QString m_favicon;

    void tryNextFavicon();
    static QString computeUrlHash(const QUrl& url);
    void done();

private Q_SLOTS:
    void slotFetchPageFinished(QNetworkReply*);
    void slotFetchFaviconFinished(QNetworkReply*);

Q_SIGNALS:
    void finished(LauncherApplication*, const QString& title, const QString& favicon);
};

#endif // WebScrapper_H

