# This macro is based on the GETTEXT_CREATE_TRANSLATIONS from FindGettext.cmake
# but it does not modify the .po files at build time.
#
# It creates two main targets: msgfmt and msgmerge.
#
# - msgfmt turns .po files into .gmo files. msgfmt is added as a dependency to
#   the all target. The .gmo files are then installed at the right place.
#
# - msgmerge updates the .po files from the .pot file. It must be called
#   manually.
#
# Arguments: .pot file, then all .po files
macro(create_translation_targets _potFile)
    set(_gmoFiles)
    get_filename_component(_potBasename ${_potFile} NAME_WE)
    get_filename_component(_absPotFile ${_potFile} ABSOLUTE)

    add_custom_target(msgmerge)

    foreach(_currentPoFile ${ARGN})
        get_filename_component(_absFile ${_currentPoFile} ABSOLUTE)
        get_filename_component(_lang ${_absFile} NAME_WE)
        set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

        # Build a .gmo file from the current .po
        add_custom_command(
            OUTPUT ${_gmoFile}
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
            DEPENDS ${_absFile}
            )
        set(_gmoFiles ${_gmoFiles} ${_gmoFile})

        # Extend the msgmerge target to merge the current .po
        add_custom_target(msgmerge-${_lang}
            COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
            DEPENDS ${_absPotFile} ${_absFile}
            )
        add_dependencies(msgmerge msgmerge-${_lang})

        # Install .gmo
        install(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
    endforeach(_currentPoFile)

    # Create the msgfmt target, building all .gmo files
    add_custom_target(msgfmt ALL DEPENDS ${_gmoFiles})
endmacro(create_translation_targets)

file(GLOB PO_FILES *.po)
set(GETTEXT_PACKAGE "unity-2d")

create_translation_targets(${GETTEXT_PACKAGE}.pot ${PO_FILES})

add_custom_target(${GETTEXT_PACKAGE}.pot
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/update-unity-2d-pot
    )

add_custom_target(update-po)
add_dependencies(update-po ${GETTEXT_PACKAGE}.pot msgmerge)
