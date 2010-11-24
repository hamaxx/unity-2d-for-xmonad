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
#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

// Local

// Qt
#include <QObject>

struct WindowHelperPrivate;
class WindowHelper : public QObject
{
Q_OBJECT
public:
    WindowHelper(QObject* parent);
    ~WindowHelper();

    void setXid(uint);

    bool isMaximized() const;

Q_SIGNALS:
    void stateChanged();

private:
    WindowHelperPrivate* const d;
};

#endif /* WINDOWHELPER_H */
