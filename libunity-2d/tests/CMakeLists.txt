macro(libunity_2d_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(${_test} ${_test})
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            unity-2d
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)

    add_custom_target(libunity-2d_check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS ${_test_list})
endmacro(libunity_2d_tests)

include_directories(
    ${libunity-2d_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    )

enable_testing()

libunity_2d_tests(
    paneltest
    )
