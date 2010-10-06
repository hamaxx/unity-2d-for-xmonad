#ifndef UNITYAPPLICATIONSPLUGIN_H
#define UNITYAPPLICATIONSPLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class UnityApplicationsPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri);
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
};


#endif // UNITYAPPLICATIONSPLUGIN_H
