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
#include <QRectF>

struct _WnckWindow;

class WindowsIntersectMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRectF monitoredArea READ monitoredArea WRITE setMonitoredArea
                                    NOTIFY monitoredAreaChanged)
    Q_PROPERTY(bool intersects READ intersects NOTIFY intersectsChanged)

public:
    WindowsIntersectMonitor();
    ~WindowsIntersectMonitor();

    QRectF monitoredArea() const;
    void setMonitoredArea(const QRectF& monitoredArea);
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
    QRectF m_monitoredArea;
    bool m_intersects;
};

#endif /* WINDOWSINTERSECTMONITOR_H */
