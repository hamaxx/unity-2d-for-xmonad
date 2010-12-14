#ifndef DashDeclarativeView_H
#define DashDeclarativeView_H

#include <QDeclarativeView>

class DashDeclarativeView : public QDeclarativeView
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QRect screenGeometry READ screenGeometry NOTIFY screenGeometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)

public:
    explicit DashDeclarativeView();

    /* getters */
    bool active() const;
    const QRect screenGeometry() const;
    const QRect availableGeometry() const;

    /* setters */
    Q_SLOT void setActive(bool active);

    /* methods */
    // FIXME: add a parameter to activate a specific section (filter)
    Q_INVOKABLE void activatePlaceEntry(const QString& file, const QString& entry);
    Q_INVOKABLE void activateHome();

signals:
    void activeChanged(bool);
    void screenGeometryChanged();
    void availableGeometryChanged();

public slots:
    void fitToAvailableSpace(int screen);

private:
    void forceActivateWindow();
    void focusOutEvent(QFocusEvent* event);

    bool m_active;
};

#endif // DashDeclarativeView_H
