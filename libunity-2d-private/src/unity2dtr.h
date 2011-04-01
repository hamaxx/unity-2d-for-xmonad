/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
#ifndef UNITY2DTR_H
#define UNITY2DTR_H

// Qt
#include <QObject>

class QString;
class QDeclarativeContext;

namespace Unity2dTr
{

/**
 * Installs our gettext catalog
 */
void init(const char* domain, const char* localeDir);

/**
 * Add an object named "u2d" to context.
 *
 * One can then get translations with u2d.tr("english text")
 */
void qmlInit(QDeclarativeContext* context);

/**
 * @internal
 */
class QmlHelper : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QString tr(const QString&, const QString& domain = QString());
    Q_INVOKABLE QString tr(const QString& singular, const QString& plural, int n, const QString& domain = QString());
};

} // namespace

/**
 * Translate the string text.
 *
 * If domain is NULL, the default domain name (set in init(…)) is used instead.
 */
QString u2dTr(const char* text, const char* domain = NULL);

/**
 * Plural version of u2dTr. Should be called like this:
 *
 * u2dTr("%n file", "%n files", count, domain)
 */
QString u2dTr(const char* singular, const char* plural, int n, const char* domain = NULL);

#endif /* UNITY2DTR_H */
