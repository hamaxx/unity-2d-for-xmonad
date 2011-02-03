#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include <QEventLoop>
#include <QObject>

class SignalWaiter : public QObject
{
    Q_OBJECT

public:
    SignalWaiter(QObject *parent = 0);
    bool waitForSignal(const QObject *sender, const char *signal, int timeout = 0);

private Q_SLOTS:
    void succeed();
    void fail();

private:
    QEventLoop m_loop;
};

#endif // SIGNALWAITER_H
