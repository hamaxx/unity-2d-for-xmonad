project(unity-2d-places)

# Source
add_subdirectory(app)

file(GLOB places_QML *.qml *.js)

# Install
install(FILES ${places_QML}
    DESTINATION ${UNITY_2D_DIR}/places
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/places
    )
