#ifndef LIBLAUNCHERPLUGIN_H
#define LIBLAUNCHERPLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class LibLauncherPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri);
};


#endif // LIBLAUNCHERPLUGIN_H
