project(unity-qt-places)

# Source
add_subdirectory(QtDee)
add_subdirectory(app)
add_subdirectory(UnityPlaces)

file(GLOB places_QML *.qml)

# Install
install(FILES ${places_QML}
    DESTINATION ${UNITY_QT_DIR}/places
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_QT_DIR}/places
    )
