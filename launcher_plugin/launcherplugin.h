#ifndef LAUNCHERPLUGIN_H
#define LAUNCHERPLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class LauncherPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri);
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
};


#endif // LAUNCHERPLUGIN_H
