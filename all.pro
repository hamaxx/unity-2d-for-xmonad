TEMPLATE = subdirs

SUBDIRS = UnityApplications launcher

INSTALL_PATH = /usr/share/unity-qt

qmls.files = *.qml
qmls.path = $$INSTALL_PATH

artwork.files = artwork/*.png artwork/*.jpg
artwork.path = $$INSTALL_PATH/artwork

autostart.files = unity-qt-launcher.desktop
autostart.path = /etc/xdg/xdg-unity-qt/autostart

INSTALLS = qmls artwork autostart

