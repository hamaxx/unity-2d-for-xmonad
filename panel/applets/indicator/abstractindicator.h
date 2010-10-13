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
#ifndef ABSTRACTINDICATOR_H
#define ABSTRACTINDICATOR_H

// Local

// Qt
#include <QObject>

class QAction;

class AbstractIndicator : public QObject
{
    Q_OBJECT
public:
    AbstractIndicator(QObject* parent=0);
    ~AbstractIndicator();

    /**
     * Called when the indicator has been constructed and its owner is connected to signals.
     * It's the right place to emit actionAdded()
     */
    virtual void init();

Q_SIGNALS:
    void actionAdded(QAction*);
    void actionRemoved(QAction*);

private:
    Q_DISABLE_COPY(AbstractIndicator)
};

#endif /* ABSTRACTINDICATOR_H */
