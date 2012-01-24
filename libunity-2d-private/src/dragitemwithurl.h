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

#ifndef DeclarativeDragItemWithUrl_H
#define DeclarativeDragItemWithUrl_H

#include "dragitem.h"

// Qt
#include <QString>
#include <QUrl>

/* A specialized DragItem with a 'url' property that knows how to convert
   application:// URLs into the corresponding file:// URLs. */
class DeclarativeDragItemWithUrl : public DeclarativeDragItem
{
    Q_OBJECT

    /* The URL cannot be a QUrl due to the malformed URIs returned by the place
    backends (see https://bugs.launchpad.net/unity-place-applications/+bug/784478). */
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    DeclarativeDragItemWithUrl(QDeclarativeItem* parent=0);
    ~DeclarativeDragItemWithUrl();

    // getters and setters
    const QString& url() const;
    void setUrl(const QString& url);

protected:
    // overridden
    QMimeData* mimeData() const;

private:
    static QUrl decodeUri(const QString& uri);

Q_SIGNALS:
    void urlChanged(const QString&);

private:
    QString m_url;
};

#endif // DeclarativeDragItemWithUrl_H

