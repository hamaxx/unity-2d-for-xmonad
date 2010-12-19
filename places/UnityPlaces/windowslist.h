#ifndef WINDOWSLIST_H
#define WINDOWSLIST_H

#include <QAbstractListModel>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QtDeclarative/qdeclarative.h>

class WindowInfo;

class WindowsList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged);
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged);
    Q_PROPERTY(unsigned long applicationId READ applicationId NOTIFY applicationIdChanged);

public:
    WindowsList(QObject *parent = 0);
    ~WindowsList();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    int count() const { return rowCount(); }
    bool loaded() const { return m_loaded; }
    unsigned long applicationId() const { return m_applicationId; }

    Q_INVOKABLE void load(unsigned long applicationId);
    Q_INVOKABLE void unload();

signals:
    void countChanged(int count);
    void applicationIdChanged(unsigned long applicationId);
    void loadedChanged(bool loaded);

private:
    QList<WindowInfo*> m_windows;
    unsigned long m_applicationId;
    bool m_loaded;
};

QML_DECLARE_TYPE(WindowsList)

#endif // WINDOWSLIST_H
