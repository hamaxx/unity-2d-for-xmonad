macro(launcher_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(NAME ${_test} 
            COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/tests/misc/run-with-xvfb.sh ./${_test}
            )
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            uqlauncher
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)
endmacro(launcher_tests)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/../app
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    ${QTGCONF_INCLUDE_DIRS}
    )

launcher_tests(
# FIXME: test need a mock dbus object to message with via dbus-test-runner
#    launcherviewtest
    )

