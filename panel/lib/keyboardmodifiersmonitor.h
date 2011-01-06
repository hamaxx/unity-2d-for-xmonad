/*
 * This file is part of unity-qt
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef KEYBOARDMODIFIERMONITOR_H
#define KEYBOARDMODIFIERMONITOR_H

// Local
#include <unityqtapplication.h>

// Qt

struct KeyboardModifiersMonitorPrivate;

/**
 * This class monitor keyboard modifiers. It is able to track changes even if
 * the active window does not belong to the application.
 *
 * You *must* use UnityQtApplication to be able to use this class.
 */
class KeyboardModifiersMonitor : public QObject, protected AbstractX11EventFilter
{
Q_OBJECT
public:
    KeyboardModifiersMonitor(QObject* parent=0);
    ~KeyboardModifiersMonitor();

    Qt::KeyboardModifiers keyboardModifiers() const;

Q_SIGNALS:
    void keyboardModifiersChanged(Qt::KeyboardModifiers);

protected:
    bool x11EventFilter(XEvent*);

private:
    KeyboardModifiersMonitorPrivate* const d;
};

#endif /* KEYBOARDMODIFIERMONITOR_H */
