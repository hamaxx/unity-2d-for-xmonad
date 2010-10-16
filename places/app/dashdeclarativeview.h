#ifndef DashDeclarativeView_H
#define DashDeclarativeView_H

#include <QDeclarativeView>

class DashDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
public:
    explicit DashDeclarativeView();
    void closeEvent(QCloseEvent* event);

    /* getters */
    bool active() const;

    /* setters */
    Q_SLOT void setActive(bool active);

signals:
    void activeChanged(bool);

public slots:
    void fitToAvailableSpace(int screen);

private:
    void forceActivateWindow();
    void focusOutEvent(QFocusEvent* event);

    bool m_active;
};

#endif // DashDeclarativeView_H
