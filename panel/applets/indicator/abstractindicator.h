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

    QAction* action() const;

private:
    Q_DISABLE_COPY(AbstractIndicator)
    QAction* m_action;
};

#endif /* ABSTRACTINDICATOR_H */
