Changes from standard unity-2d
============================

* Display both application name and window title in panel
* Move launcher under panel so it doesn't cover it
* Load panel only on the primary screen
* Don't filter apps in lagacy tray
* Changed spread(aplication and workspace switcher) behaviour so it's useful in xmonad.
* Simplified spred layout and animations so it works much faster

Compiling unity-2d
==================

    cmake . && make

Installing unity-2d-for-xmonad
==============================

    sudo apt-get install unity-2d
    sudo make install

Running separate components
===========================

    ./shell/app/unity-2d-shell
    ./panel/app/unity-2d-panel
    ./places/app/unity-2d-places

What it looks like
==================

![Screen shot 1](/hamaxx/unity-2d-for-xmonad/raw/master/docs/ss1.png)
![Screen shot 2](/hamaxx/unity-2d-for-xmonad/raw/master/docs/ss2.png)

Notes on the Dash
=================

The dash exposes a D-Bus property for activation and deactivation:

com.canonical.Unity2d /Dash com.canonical.Unity2d.Dash.active true|false

