TEMPLATE = subdirs

SUBDIRS = UnityApplications launcher

INSTALL_PATH = /usr/share/unity-qt

qmls.files = *.qml
qmls.path = $$INSTALL_PATH

artwork.files = artwork/*.png artwork/*.jpg
artwork.path = $$INSTALL_PATH/artwork

INSTALLS = qmls artwork

