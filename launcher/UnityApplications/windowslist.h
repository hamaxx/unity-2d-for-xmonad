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
public:
    WindowsList(QObject *parent = 0);
    ~WindowsList() {}

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    Q_PROPERTY(int count READ count NOTIFY countChanged);
    int count() const { return rowCount(); }

    void setGrabber(WindowGrabber *grabber) { m_capture = grabber; }

signals:
    void countChanged(int count);

private:
    QList<WindowInfo*> m_windows;
    WindowGrabber *m_capture;

//private slots:
//    void onBamfViewOpened(BamfView* bamf_view);
//    void onBamfViewClosed(BamfView* bamf_view);
};

QML_DECLARE_TYPE(WindowsList)

#endif // WINDOWSLIST_H
