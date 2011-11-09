project(unity-2d-places)

# Source
add_subdirectory(app)

file(GLOB places_QML qml/*.qml qml/*.js)

# Install
# TODO: fix this to install the merged files in the correct place
install(FILES ${places_QML}
    DESTINATION ${UNITY_2D_DIR}/places
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/places
    )
