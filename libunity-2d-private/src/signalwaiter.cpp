#include "signalwaiter.h"

#include <QTimer>
#include <debug_p.h>

SignalWaiter::SignalWaiter(QObject *parent) : QObject(parent)
{
}

/* Wait for a signal from sender to be emitted, and return true when it's emitted.
   Optionally if you specify a timeout, and the signal is not emitted before it
   does expire, the functions returns false.

   While waiting the application will not react to user input events, and they will
   all be delivered the wait finishes. Please note that other events will be delivered
   normally, so care must be taken to not enter the wait again when reacting to these
   events. See for more details:
   http://wiki.forum.nokia.com/index.php/How_to_wait_synchronously_for_a_Signal_in_Qt
*/
bool SignalWaiter::waitForSignal(const QObject *sender, const char *signal, int timeout)
{
    QObject::connect(sender, signal, this,  SLOT(succeed()));
    if (timeout > 0) {
        QTimer::singleShot(timeout, this, SLOT(fail()));
    }

    /* We pass the QEventLoop::ExcludeUserInputEvents to prevent UI input
       to happen while we are in the loop.
       Please note that this won't work on OSX:
       http://bugreports.qt.nokia.com/browse/QTBUG-15645
    */
    int result = m_loop.exec(QEventLoop::ExcludeUserInputEvents);
    if (result != 0) {
        UQ_WARNING << "Timeout expired while waiting for signal";
    }
    return (result == 0);
}

void SignalWaiter::succeed()
{
    m_loop.exit(0);
}

void SignalWaiter::fail()
{
    m_loop.exit(1);
}

#include "signalwaiter.moc"
