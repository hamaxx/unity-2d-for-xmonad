#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include <QEventLoop>
#include <QTimer>
#include <QObject>

class SignalWaiter : public QObject
{
    Q_OBJECT

public:
    SignalWaiter(QObject *parent = 0);

public:
    static bool waitForSignal(const QObject *sender, const char *signal, int timeout = 0);
    bool wait(const QObject *sender, const char *signal, int timeout = 0);

public Q_SLOTS:
    void succeed();
    void fail();

private:
    QEventLoop loop;
};

#endif // SIGNALWAITER_H
