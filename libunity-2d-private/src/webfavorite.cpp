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

#include <glib.h>

#include "webfavorite.h"

// libunity-2d
#include <gscopedpointer.h>

// Qt
#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegExp>
#include <QPixmap>
#include <QCryptographicHash>
#include <QTextDocumentFragment>

static const QString WEBFAV_STORE = QDir::homePath() + "/.local/share/applications/";
static const QString ICON_STORE = QDir::homePath() + "/.local/share/icons/";

static void check_store_exists(const QString& path)
{
    if (!QDir(path).exists()) {
        QDir().mkpath(path);
    }
}

#define check_webfav_store_exists() check_store_exists(WEBFAV_STORE)
#define check_icon_store_exists() check_store_exists(ICON_STORE)

static const QString WEBFAV_DESKTOP_ENTRY =
    "[Desktop Entry]\n"
    "Version=1.0\n"
    "Name={name}\n"
    "Exec=xdg-open \"{url}\"\n"
    "Type=Application\n"
    "Icon=emblem-web\n"
    "Categories=Network;\n"
    "MimeType=text/html;\n"
    "StartupNotify=true\n";

static const uint MAX_REDIRECTS = 6;

WebFavorite::WebFavorite(const QUrl& url, QObject* parent)
    : QObject(parent)
    , m_url(url)
    , m_redirects(0)
{
    m_desktopFile = WEBFAV_STORE + "webfav-" + computeUrlHash(url) + ".desktop";

    QString contents = WEBFAV_DESKTOP_ENTRY;
    QByteArray encoded = url.toEncoded();
    contents.replace("{name}", encoded);
    contents.replace("{url}", encoded);
    writeDesktopFile(contents.toUtf8());

    fetchPage();
}

WebFavorite::~WebFavorite()
{
}

const QString&
WebFavorite::desktopFile() const
{
    return m_desktopFile;
}

void
WebFavorite::writeDesktopFile(const QByteArray& contents) const
{
    check_webfav_store_exists();
    QFile file(m_desktopFile);
    file.open(QIODevice::WriteOnly);
    file.write(contents);
    file.close();
}

typedef GScopedPointer<GKeyFile, g_key_file_free> GKeyFilePointer;

void
WebFavorite::modifyDesktopFile(const QString& key, const QString& value) const
{
    GKeyFilePointer keyFile(g_key_file_new());
    gboolean loaded = g_key_file_load_from_file(keyFile.data(), m_desktopFile.toUtf8().constData(),
        (GKeyFileFlags) (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS), NULL);
    if (loaded) {
        g_key_file_set_string(keyFile.data(), "Desktop Entry", key.toUtf8().constData(), value.toUtf8().constData());
        QByteArray contents = g_key_file_to_data(keyFile.data(), NULL, NULL);
        writeDesktopFile(contents);
    }
}

void
WebFavorite::fetchPage()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(slotFetchPageFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(m_url));
}

void
WebFavorite::slotFetchPageFinished(QNetworkReply* reply)
{
    QNetworkAccessManager* manager = static_cast<QNetworkAccessManager*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            m_redirects++;
            if (m_redirects < MAX_REDIRECTS) {
                m_url = redirect.toUrl();
                fetchPage();
            }
        } else {
            QString data = QString::fromUtf8(reply->readAll());

            /* lookup title */
            QRegExp reTitle("<title>(.*)</title>", Qt::CaseInsensitive);
            int index = reTitle.indexIn(data);
            if (index != -1) {
                /* The title may contain HTML entities, unescape them. */
                QString title = QTextDocumentFragment::fromHtml(reTitle.cap(1).simplified()).toPlainText();
                modifyDesktopFile("Name", title);
            }

            /* lookup favicons */
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

    reply->deleteLater();
    manager->deleteLater();
}

void
WebFavorite::tryNextFavicon()
{
    if (m_current_favicon == m_favicons.end()) {
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
WebFavorite::slotFetchFaviconFinished(QNetworkReply* reply)
{
    QNetworkAccessManager* manager = static_cast<QNetworkAccessManager*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            m_redirects++;
            if (m_redirects < MAX_REDIRECTS) {
                *m_current_favicon = redirect.toUrl().toEncoded();
            } else {
                m_current_favicon++;
                m_redirects = 0;
            }
            tryNextFavicon();
        } else {
            /* Check that the data is actually an image. This will cope with
               badly configured web servers that don’t return error codes on
               non-existing files. */
            QPixmap pixmap;
            bool valid = pixmap.loadFromData(reply->readAll());
            if (valid) {
                check_icon_store_exists();
                QUrl url = reply->url();
                QString filepath = ICON_STORE + computeUrlHash(url);
                QString extension = url.path().mid(url.path().lastIndexOf("."));
                QString filename = filepath + extension;
                pixmap.save(filename);
                modifyDesktopFile("Icon", filename);
            } else {
                m_current_favicon++;
                m_redirects = 0;
                tryNextFavicon();
            }
        }
    } else {
        m_current_favicon++;
        m_redirects = 0;
        tryNextFavicon();
    }

    reply->deleteLater();
    manager->deleteLater();
}

QString
WebFavorite::computeUrlHash(const QUrl& url)
{
    return QCryptographicHash::hash(url.toEncoded(), QCryptographicHash::Md5).toHex().constData();
}

#include "webfavorite.moc"
