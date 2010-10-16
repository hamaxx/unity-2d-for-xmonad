#ifndef LAUNCHERDECLARATIVEVIEW_H
#define LAUNCHERDECLARATIVEVIEW_H

#include <QDeclarativeView>

class LauncherDeclarativeView : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit LauncherDeclarativeView();
    void closeEvent(QCloseEvent* event);

signals:

public slots:
    void fitToAvailableSpace(int screen);
    Q_SCRIPTABLE void activate();
    Q_SCRIPTABLE void deactivate();
};

#endif // LAUNCHERDECLARATIVEVIEW_H
