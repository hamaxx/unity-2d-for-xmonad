#ifndef WINDOWSLIST_H
#define WINDOWSLIST_H

#include <QAbstractListModel>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QtDeclarative/qdeclarative.h>

class WindowInfo;
class WindowGrabber;

class WindowsList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged);
    Q_PROPERTY(unsigned long applicationId READ applicationId WRITE setApplicationId NOTIFY applicationIdChanged);

public:
    WindowsList(QObject *parent = 0);
    ~WindowsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    int count() const { return rowCount(); }
    unsigned long applicationId() const { return m_applicationId; }
    void setApplicationId(unsigned long applicationId);

    void setGrabber(WindowGrabber *grabber) { m_capture = grabber; }

    Q_INVOKABLE void load();
    Q_INVOKABLE void unload();

signals:
    void countChanged(int count);
    void applicationIdChanged(unsigned long applicationId);
    void loaded();

private:
    QList<WindowInfo*> m_windows;
    WindowGrabber *m_capture;
    unsigned long m_applicationId;
};

QML_DECLARE_TYPE(WindowsList)

#endif // WINDOWSLIST_H
