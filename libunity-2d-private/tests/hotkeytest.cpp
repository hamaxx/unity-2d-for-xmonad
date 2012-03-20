/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Pawel Stolowski <pawel.stolowski@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unitytestmacro.h>
#include <hotkey.h>
#include <hotkeymonitor.h>

#include <QApplication>
#include <QX11Info>
#include <QtTestGui>

// XK_MISCELLANY and XK_LATIN1 are needed for XK_* keysymdef definitions
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/extensions/XTest.h>

/* Helper class that connects to Hotkey pressed() signal and
   records key/modifiers and total number of key presses */
class HotKeyPressReceiver: public QObject
{
    Q_OBJECT
public:
    HotKeyPressReceiver(): m_count(0) {
    }

    int count() const { return m_count; }
    void reset() { m_count = 0; }
    Qt::Key key() const { return m_key; }
    Qt::KeyboardModifiers modifiers() const { return m_modifiers; }

public Q_SLOTS:
    void onKeyPressed() {
        Hotkey* hotkey = qobject_cast<Hotkey*>(sender());

        m_key = hotkey->key();
        m_modifiers = hotkey->modifiers();
       ++m_count;
    }

 private:
    Qt::Key m_key;
    Qt::KeyboardModifiers m_modifiers;
    int m_count;
};

class HotkeyTest: public QObject
{
    static void simulateKey(KeySym keysym, KeySym modsym1, KeySym modsym2 = 0)  {
        Display *disp = QX11Info::display();

        KeyCode modcode1, modcode2;
        KeyCode keycode = XKeysymToKeycode(disp, keysym);

        XTestGrabControl (disp, True);  

        /* Generate modkey press */  
        if (modsym1 != 0)  { 
            modcode1 = XKeysymToKeycode(disp, modsym1);
            XTestFakeKeyEvent(disp, modcode1, True, 0);
        }
      
        if (modsym2 != 0)  { 
            modcode2 = XKeysymToKeycode(disp, modsym2);
            XTestFakeKeyEvent(disp, modcode2, True, 1);
        }

        /* Generate regular key press and release */  
        XTestFakeKeyEvent(disp, keycode, True, 10); 
        XTestFakeKeyEvent(disp, keycode, False, 20);
      
        /* Generate modkey release */  
        if (modsym1 != 0) { 
            XTestFakeKeyEvent(disp, modcode1, False, 40);
        }

        if (modsym2 != 0) { 
            XTestFakeKeyEvent(disp, modcode2, False, 41);
        }
      
        XSync(disp, False); 
        XTestGrabControl(disp, False); 
        
        /* Process Qt events. This is crucial since simulated keypress won't automagically trigger Qt signal */
        QApplication::processEvents();
    } 

    Q_OBJECT
private Q_SLOTS:

    void initTestCase()
    {
       for (Qt::Key key = Qt::Key_0; key <= Qt::Key_9; key = (Qt::Key) (key + 1)) {
            Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier);
            connect(hotkey, SIGNAL(pressed()), &m_rcv, SLOT(onKeyPressed()));
            hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier | Qt::ShiftModifier);
            connect(hotkey, SIGNAL(pressed()), &m_shiftrcv, SLOT(onKeyPressed()));
            hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier | Qt::KeypadModifier);
            connect(hotkey, SIGNAL(pressed()), &m_keypadrcv, SLOT(onKeyPressed()));
        }
    }

    void init()
    {
        m_rcv.reset();
        m_shiftrcv.reset();
        m_keypadrcv.reset();
    }

    /* Test super + number hotkeys */
    void testNumericKeypress()
    {
        QCOMPARE(m_rcv.count(), 0);

        simulateKey(XK_1, XK_Super_L);
        QCOMPARE(m_rcv.count(), 1);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_rcv.key(), Qt::Key_1);
        QCOMPARE(m_rcv.modifiers(), Qt::MetaModifier);
        
        simulateKey(XK_9, XK_Super_L);
        QCOMPARE(m_rcv.count(), 2);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_rcv.key(), Qt::Key_9);
        QCOMPARE(m_rcv.modifiers(), Qt::MetaModifier);

        simulateKey(XK_0, XK_Super_L);
        QCOMPARE(m_rcv.count(), 3);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_rcv.key(), Qt::Key_0);
        QCOMPARE(m_rcv.modifiers(), Qt::MetaModifier);
    }
    
    /* Test for super + shift + number hotkeys */
    void testShiftNumericKeypress()
    {
        QCOMPARE(m_shiftrcv.count(), 0);
       
        simulateKey(XK_1, XK_Super_L, XK_Shift_L);
        QCOMPARE(m_shiftrcv.count(), 1);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.key(), Qt::Key_1);
        QCOMPARE(m_shiftrcv.modifiers(), Qt::MetaModifier | Qt::ShiftModifier);
        
        simulateKey(XK_9, XK_Super_L, XK_Shift_L);
        QCOMPARE(m_shiftrcv.count(), 2);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.key(), Qt::Key_9);
        QCOMPARE(m_shiftrcv.modifiers(), Qt::MetaModifier | Qt::ShiftModifier);

        simulateKey(XK_0, XK_Super_L, XK_Shift_L);
        QCOMPARE(m_shiftrcv.count(), 3);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_keypadrcv.count(), 0);
        QCOMPARE(m_shiftrcv.key(), Qt::Key_0);
        QCOMPARE(m_shiftrcv.modifiers(), Qt::MetaModifier | Qt::ShiftModifier);
    }
   
    /* Test for super + numeric keyboard hotkeys */
    void testNumpadNumericKeypress()
    {
        QCOMPARE(m_keypadrcv.count(), 0);
       
        simulateKey(XK_KP_1, XK_Super_L);
        QCOMPARE(m_keypadrcv.count(), 1);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_keypadrcv.key(), Qt::Key_1);
        QCOMPARE(m_keypadrcv.modifiers(), Qt::MetaModifier | Qt::KeypadModifier);
        
        simulateKey(XK_KP_9, XK_Super_L);
        QCOMPARE(m_keypadrcv.count(), 2);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_keypadrcv.key(), Qt::Key_9);
        QCOMPARE(m_keypadrcv.modifiers(), Qt::MetaModifier | Qt::KeypadModifier);

        simulateKey(XK_KP_0, XK_Super_L);
        QCOMPARE(m_keypadrcv.count(), 3);
        QCOMPARE(m_rcv.count(), 0);
        QCOMPARE(m_shiftrcv.count(), 0);
        QCOMPARE(m_keypadrcv.key(), Qt::Key_0);
        QCOMPARE(m_keypadrcv.modifiers(), Qt::MetaModifier | Qt::KeypadModifier);
    }
        
private:
    HotKeyPressReceiver m_rcv; // monitor for meta+number hotkeys
    HotKeyPressReceiver m_shiftrcv; //monitor for meta+shift+number hotkeys
    HotKeyPressReceiver m_keypadrcv; //monitor for meta+numpad hotkeys
};
 
UAPP_TEST_MAIN(HotkeyTest)

#include "hotkeytest.moc"

