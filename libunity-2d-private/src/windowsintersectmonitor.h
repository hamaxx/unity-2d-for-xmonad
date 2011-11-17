/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
 * - Ugo Riboni <ugo.riboni@canonical.com>
 *
 * License: GPL v3
 */
#ifndef WINDOWSINTERSECTMONITOR_H
#define WINDOWSINTERSECTMONITOR_H

// Qt
#include <QObject>
#include <QRect>

struct _WnckWindow;

class WindowsIntersectMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRect monitoredArea READ monitoredArea WRITE setMonitoredArea
                                   NOTIFY monitoredAreaChanged)
    Q_PROPERTY(bool intersects READ intersects NOTIFY intersectsChanged)

public:
    WindowsIntersectMonitor();
    ~WindowsIntersectMonitor();

    QRect monitoredArea() const;
    void setMonitoredArea(const QRect& monitoredArea);
    bool intersects() const;

Q_SIGNALS:
    void monitoredAreaChanged();
    void intersectsChanged();

private Q_SLOTS:
    void updateIntersect();
    void updateActiveWindowConnections();

private:
    Q_DISABLE_COPY(WindowsIntersectMonitor);
    void disconnectFromGSignals();

    struct _WnckWindow* m_activeWindow;
    QRect m_monitoredArea;
    bool m_intersects;
};

#endif /* WINDOWSINTERSECTMONITOR_H */
