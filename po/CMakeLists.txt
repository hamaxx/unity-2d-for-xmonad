file(GLOB PO_FILES *.po)

add_custom_target(update-po
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/update-unity-2d-pot
    )

gettext_create_translations(unity-2d.pot ALL ${PO_FILES})
