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

#ifndef WebFavorite_H
#define WebFavorite_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QStringList>

class QNetworkReply;

class WebFavorite : public QObject
{
    Q_OBJECT

public:
    WebFavorite(const QUrl& url, QObject* parent=0);
    ~WebFavorite();

    const QString& desktopFile() const;

private:
    QUrl m_url;
    QString m_desktopFile;

    uint m_redirects;
    QStringList m_favicons;
    QStringList::iterator m_current_favicon;

    static QString computeUrlHash(const QUrl& url);

    void writeDesktopFile(const QByteArray& contents) const;
    void modifyDesktopFile(const QString& key, const QString& value) const;

    void fetchPage();
    void tryNextFavicon();

private Q_SLOTS:
    void slotFetchPageFinished(QNetworkReply*);
    void slotFetchFaviconFinished(QNetworkReply*);
};

#endif // WebFavorite_H

