function launchApplicationFromDesktopFile(desktopFile, parentItem) {
    var application = Qt.createQmlObject("import UnityApplications 1.0; QLauncherApplication {desktop_file: \"%1\"}".arg(desktopFile),
                                         parentItem, "");
    application.launch()
    application.destroy()
}
