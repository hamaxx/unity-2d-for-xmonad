macro(launcher_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(${_test} ${_test})
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            uqlauncher
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)

    add_custom_target(launcher_check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS ${_test_list})
    add_dependencies(check launcher_check)
endmacro(launcher_tests)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../app
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    ${QTGCONF_INCLUDE_DIRS}
    )

enable_testing()

launcher_tests(
    launcherviewtest
    )

