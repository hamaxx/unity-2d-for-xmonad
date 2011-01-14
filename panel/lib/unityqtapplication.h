/*
 * Unity2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef UNITY2DAPPLICATION_H
#define UNITY2DAPPLICATION_H

// Qt
#include <QApplication>

class Unity2dApplication;

class AbstractX11EventFilter
{
public:
    virtual ~AbstractX11EventFilter();

protected:
    virtual bool x11EventFilter(XEvent*) = 0;

    friend class Unity2dApplication;
};

class Unity2dApplication : public QApplication
{
Q_OBJECT
public:
    Unity2dApplication(int& argc, char** argv);

    void installX11EventFilter(AbstractX11EventFilter*);
    void removeX11EventFilter(AbstractX11EventFilter*);

    /**
     * Note: This function will return a null pointer if you did not use a Unity2dApplication in your application!
     */
    static Unity2dApplication* instance();

protected:
    bool x11EventFilter(XEvent*);

private:
    QList<AbstractX11EventFilter*> m_x11EventFilters;
};

#endif // UNITY2DAPPLICATION_H
