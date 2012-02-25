macro(panel_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(NAME ${_test} 
            COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/tests/misc/run-with-xvfb.sh ${_test}
            )
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            panelplugin-homebutton
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)

    add_custom_target(panel_check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS ${_test_list})
    add_dependencies(check panel_check)
endmacro(panel_tests)

include_directories(
    ${panelplugin-homebutton_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    )

#panel_tests(
#    )
