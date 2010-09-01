TEMPLATE = subdirs

SUBDIRS = launcher_plugin launcher

INSTALL_PATH = /usr/share/unity-qt

qmls.files = *.qml
qmls.path = $$INSTALL_PATH

artwork.files = artwork/*.png
artwork.path = $$INSTALL_PATH/artwork

INSTALLS = qmls artwork

