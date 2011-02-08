# Common settings
include_directories(
    ${libunity-2d_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    )

enable_testing()

set(LIBUNITY_2D_TEST_DIR ${libunity-2d_BINARY_DIR}/tests)

# Unit-tests
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
    add_dependencies(check libunity-2d_check)
endmacro(libunity_2d_tests)

libunity_2d_tests(
    keyboardmodifiersmonitortest
    paneltest
    )

# gnomesessionclienttest
add_executable(gnomesessionclienttesthelper
    gnomesessionclienttesthelper.cpp
    )
target_link_libraries(gnomesessionclienttesthelper
    unity-2d
    )

configure_file(gnomesessionclienttesthelper.desktop.in
    gnomesessionclienttesthelper.desktop @ONLY)
configure_file(gnomesessionclienttest.sh.in
    gnomesessionclienttest.sh @ONLY)

add_test(NAME gnomesessionclient_check
    COMMAND /bin/sh gnomesessionclienttest.sh
    )
add_dependencies(libunity-2d_check gnomesessionclient_check)
