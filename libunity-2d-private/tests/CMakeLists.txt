# Common settings
configure_file(config-test.h.in config-test.h)

include_directories(
    ${libunity-2d-private_SOURCE_DIR}/src
    ${libunity-2d-private_SOURCE_DIR}/Unity2d
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${QT_QTTEST_INCLUDE_DIR}
    )

enable_testing()

set(LIBUNITY_2D_TEST_DIR ${libunity-2d-private_BINARY_DIR}/tests)

# Unit-tests
macro(libunity_2d_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(${_test} ${_test})
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            unity-2d-private
            unity-2d-private-qml
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)

    add_custom_target(libunity-2d-private_check COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS ${_test_list})
    add_dependencies(check libunity-2d-private_check)
endmacro(libunity_2d_tests)

libunity_2d_tests(
    gconnectortest
    keyboardmodifiersmonitortest
    paneltest
    unity2dtrtest
    launchermenutest
    listaggregatormodeltest
    qsortfilterproxymodeltest
    )

add_custom_target(unity2dtr_po COMMAND
    mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/locale/fr/LC_MESSAGES/
    && ${GETTEXT_MSGFMT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/fr.po
        -o ${CMAKE_CURRENT_BINARY_DIR}/locale/fr/LC_MESSAGES/unity2dtrtest.mo
    )

add_dependencies(unity2dtrtest unity2dtr_po)

# gnomesessionclienttest
add_executable(gnomesessionclienttesthelper
    gnomesessionclienttesthelper.cpp
    )
target_link_libraries(gnomesessionclienttesthelper
    unity-2d-private
    )

configure_file(gnomesessionclienttesthelper.desktop.in
    gnomesessionclienttesthelper.desktop @ONLY)
configure_file(gnomesessionclienttest.sh.in
    gnomesessionclienttest.sh @ONLY)

add_test(NAME gnomesessionclient_check
    COMMAND /bin/sh gnomesessionclienttest.sh
    )
add_dependencies(libunity-2d-private_check gnomesessionclient_check)

# mouseareademo
add_executable(mouseareademo
    mouseareademo.cpp
    )
target_link_libraries(mouseareademo
    unity-2d-private
    )
