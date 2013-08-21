## Changes from standard unity-2d

* __Xmonad workspace indicator!__
* __Display both application name and window title in panel__
* Move launcher under panel so it doesn't cover it
* Load panel only on the primary screen
* Don't filter apps in lagacy tray
* Changed spread(aplication and workspace switcher) behaviour so it's useful in xmonad.
* Simplified spred layout and animations so it works much faster

## Setup Xmonad on Ubuntu

Start with [this tutorial](http://markhansen.co.nz/xmonad-ubuntu-oneiric/)

### Clone this repository

    git clone git://github.com/hamaxx/unity-2d-for-xmonad.git

### Install unity-2d from Ubuntu repository

    sudo apt-get install unity-2d

### Install build dependencies for unity-2d from Ubuntu repository

    sudo apt-get build-dep unity-2d

### Compile unity-2d-for-xmonad

    cd unity-2d-for-xmonad
    cmake . && make

### Running separate components

    ./launcher/app/unity-2d-launcher
    ./panel/app/unity-2d-panel
    ./places/app/unity-2d-places

### Install unity-2d-for-xmonad

    sudo make install

### Enabling Xmonad workspace indicator (Inspired by [xmonad-log-applet for gnome panel](http://uhsure.com/xmonad-log-applet.html))

1. Use my xmonad.hs config file from xmonad-files folder
2. If you have problems with DBus error message, use the version in xmonad-files (Install with sudo cabal install)
3. Open dconf-editor, go to "com -> canonical -> unity-2d -> panel " and check the xmonadlog field or add "xmonadlog" to applets field.
4. Restart unity-2d-panel

## What it looks like

![Screen shot 1](/docs/ss2.png)
![Screen shot 2](/docs/ss3.png)

### If you experience problems with Xmonad not redrawing windows while switching workspaces, try disabling desktop icons

    gsettings set org.gnome.desktop.background show-desktop-icons false
