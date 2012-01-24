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
#ifndef CROPPEDLABEL_H
#define CROPPEDLABEL_H

// Local

// Qt
#include <QLabel>

class GConfItemQmlWrapper;

/**
 * This label makes sure minimumSizeHint() is not set. This ensures the applet
 * does not get wider if a window title is very long
 */
class CroppedLabel : public QLabel
{
    Q_OBJECT
public:
    CroppedLabel(QWidget* parent = 0);

    QSize minimumSizeHint() const;

protected:
    void paintEvent(QPaintEvent*);

private Q_SLOTS:
    void onWindowTitleFontNameChanged();

private:
    GConfItemQmlWrapper *m_gconfItem;
    QString m_windowTitleFontName;
};

#endif /* CROPPEDLABEL_H */
