/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * License: GPL v3
 */

#ifndef SHOWDESKTOPMONITOR_H
#define SHOWDESKTOPMONITOR_H

// Qt
#include <QObject>

class ShowDesktopMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool shown READ shown NOTIFY shownChanged)

public:
    ShowDesktopMonitor();
    ~ShowDesktopMonitor();

    bool shown() const;

Q_SIGNALS:
    void shownChanged();

private Q_SLOTS:
    void onShowingDesktopChanged();

private:
    Q_DISABLE_COPY(ShowDesktopMonitor);
};

#endif /* SHOWDESKTOPMONITOR_H */
