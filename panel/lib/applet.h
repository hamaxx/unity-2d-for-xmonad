/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: LGPL v3
 */
#ifndef APPLET_H
#define APPLET_H

// Local

// Qt
#include <QX11EmbedWidget>

namespace UnityQt
{

struct AppletPrivate;
class Applet : public QX11EmbedWidget
{
Q_OBJECT
public:
    Applet();
    ~Applet();

private:
    Q_DISABLE_COPY(Applet)
    AppletPrivate* const d;
};

typedef Applet* (*AppletCreatorFunction)();
template <class T>
Applet* appletCreator()
{
    return new T;
}
int appletMain(int argc, char** argv, AppletCreatorFunction);

} // namespace

#define APPLET_MAIN(APPLET_CLASS) \
int main(int argc, char** argv) \
{ \
    return UnityQt::appletMain(argc, argv, UnityQt::appletCreator<APPLET_CLASS>); \
}

#endif /* APPLET_H */
