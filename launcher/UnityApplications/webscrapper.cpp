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

#include "webscrapper.h"

#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegExp>
#include <QCryptographicHash>

static const QString REL_ICON_STORE = ".local/share/icons/";
static const QString ABS_ICON_STORE = QDir::homePath() + "/" + REL_ICON_STORE;

static void check_icon_store_exists()
{
    QDir store(ABS_ICON_STORE);
    if (!store.exists()) {
        QDir::home().mkpath(REL_ICON_STORE);
    }
}

static const uint MAX_REDIRECTS = 6;

WebScrapper::WebScrapper(LauncherApplication* application, const QUrl& url, QObject* parent)
    : QObject(parent)
    , m_application(application)
    , m_url(url)
    , m_redirects(0)
{
}

WebScrapper::~WebScrapper()
{
}

void
WebScrapper::fetchAndScrap()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(slotFetchPageFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(m_url));
}

void
WebScrapper::slotFetchPageFinished(QNetworkReply* reply)
{
    QNetworkAccessManager* manager = static_cast<QNetworkAccessManager*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            m_redirects++;
            if (m_redirects < MAX_REDIRECTS) {
                m_url = redirect.toUrl();
                fetchAndScrap();
            }
        }
        else {
            QString data = QString::fromUtf8(reply->readAll());

            /* lookup title */
            QRegExp reTitle("<title>(.*)</title>", Qt::CaseInsensitive);
            int index = reTitle.indexIn(data);
            if (index != -1) {
                m_title = reTitle.cap(1).simplified();
            }

            /* favicons lookup */
            QRegExp reFavicon1("<link rel=\"apple-touch-icon\".*href=\"(.*)\"", Qt::CaseInsensitive);
            reFavicon1.setMinimal(true);
            index = reFavicon1.indexIn(data);
            if (index != -1) {
                m_favicons << reFavicon1.cap(1);
            }
            QRegExp reFavicon2("<link rel=\"(shortcut )?icon\".*href=\"(.*)\"", Qt::CaseInsensitive);
            reFavicon2.setMinimal(true);
            index = reFavicon2.indexIn(data);
            if (index != -1) {
                m_favicons << reFavicon2.cap(2);
            }
            m_favicons << "/apple-touch-icon.png";
            m_favicons << "/favicon.ico";

            m_current_favicon = m_favicons.begin();
            m_redirects = 0;
            tryNextFavicon();
        }
    }
    else {
        done();
    }

    reply->deleteLater();
    manager->deleteLater();
}

void
WebScrapper::tryNextFavicon()
{
    if (m_current_favicon == m_favicons.end()) {
        done();
        return;
    }

    QUrl url(*m_current_favicon);
    if (url.isRelative()) {
        url = m_url.resolved(url);
    }

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(slotFetchFaviconFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(url));
}

void
WebScrapper::slotFetchFaviconFinished(QNetworkReply* reply)
{
    QNetworkAccessManager* manager = static_cast<QNetworkAccessManager*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            m_redirects++;
            if (m_redirects < MAX_REDIRECTS) {
                *m_current_favicon = redirect.toUrl().toEncoded();
            }
            else {
                m_current_favicon++;
                m_redirects = 0;
            }
            tryNextFavicon();
        }
        else {
            QUrl url = reply->url();

            check_icon_store_exists();
            QString filename = ABS_ICON_STORE + computeUrlHash(url);
            QString extension = url.path().mid(url.path().lastIndexOf("."));
            QFile file(filename + extension);
            file.open(QIODevice::WriteOnly);
            file.write(reply->readAll());
            file.close();

            m_favicon = file.fileName();
            done();
        }
    }
    else {
        m_current_favicon++;
        m_redirects = 0;
        tryNextFavicon();
    }

    reply->deleteLater();
    manager->deleteLater();
}

QString
WebScrapper::computeUrlHash(const QUrl& url)
{
    return QCryptographicHash::hash(url.toEncoded(), QCryptographicHash::Md5).toHex().constData();
}

void
WebScrapper::done()
{
    Q_EMIT finished(m_application, m_title, m_favicon);
}

