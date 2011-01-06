/*
 * UnityQt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef UNITYQTAPPLICATION_H
#define UNITYQTAPPLICATION_H

// Qt
#include <QApplication>

class AbstractX11EventFilter
{
public:
    virtual ~AbstractX11EventFilter();
    virtual bool x11EventFilter(XEvent*) = 0;
};

class UnityQtApplication : public QApplication
{
public:
    UnityQtApplication(int& argc, char** argv);

    void installX11EventFilter(AbstractX11EventFilter*);
    void removeX11EventFilter(AbstractX11EventFilter*);

    /**
     * Note: This function will return a broken pointer if you did not use a UnityQtApplication in your application!
     */
    static UnityQtApplication* instance();

protected:
    bool x11EventFilter(XEvent*);

private:
    QList<AbstractX11EventFilter*> m_x11EventFilters;
};

#endif // UNITYQTAPPLICATION_H
