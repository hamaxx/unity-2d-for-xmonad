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
#ifndef APPLETCONTAINER_H
#define APPLETCONTAINER_H

// Local

// Qt
#include <QX11EmbedContainer>

namespace UnityQt
{

struct AppletContainerPrivate;
class AppletContainer : public QX11EmbedContainer
{
Q_OBJECT
public:
    AppletContainer(const QString& appletName);
    ~AppletContainer();

protected:
    virtual void showEvent(QShowEvent*);
    virtual bool x11Event(XEvent*);

private:
    AppletContainerPrivate* const d;
};

} // namespace

#endif /* APPLETCONTAINER_H */
