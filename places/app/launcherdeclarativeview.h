#ifndef LAUNCHERDECLARATIVEVIEW_H
#define LAUNCHERDECLARATIVEVIEW_H

#include <QDeclarativeView>

class LauncherDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
public:
    explicit LauncherDeclarativeView();
    void closeEvent(QCloseEvent* event);

    /* getters */
    bool active() const;

    /* setters */
    void setActive(bool active);

signals:
    void activeChanged(bool);

public slots:
    void fitToAvailableSpace(int screen);
};

#endif // LAUNCHERDECLARATIVEVIEW_H
