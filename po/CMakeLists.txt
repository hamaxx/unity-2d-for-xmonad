set(DOMAIN unity-2d)

find_program(GETTEXT_XGETTEST_EXECUTABLE xgettext)

add_custom_target(update-pot
    COMMAND find ${CMAKE_SOURCE_DIR} -name '*.cpp' > lst
    COMMAND ${GETTEXT_XGETTEST_EXECUTABLE}
        -o ${CMAKE_CURRENT_SOURCE_DIR}/${DOMAIN}.pot
        --files-from lst
        --qt
        --c++
        --keyword=tr
        --force-po
    COMMAND rm lst
        )

file(GLOB PO_FILES *.po)

gettext_create_translations(${CMAKE_CURRENT_SOURCE_DIR}/${DOMAIN}.pot ALL ${PO_FILES})
