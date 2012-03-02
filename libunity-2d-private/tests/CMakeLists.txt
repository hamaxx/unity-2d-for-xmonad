# Common settings
configure_file(config-test.h.in config-test.h)

include_directories(
    ${libunity-2d-private_SOURCE_DIR}/src
    ${libunity-2d-private_SOURCE_DIR}/Unity2d
    ${CMAKE_SOURCE_DIR}/tests/misc
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${QT_QTTEST_INCLUDE_DIR}
    ${X11_XTest_INCLUDE_PATH}
    )

set(LIBUNITY_2D_TEST_DIR ${libunity-2d-private_BINARY_DIR}/tests)

# Unit-tests (all run with Xvfb)
macro(libunity_2d_tests)
    set(_test_list "")
    foreach(_test ${ARGN})
        add_test(NAME ${_test} 
            COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/tests/misc/run-with-xvfb.sh ./${_test}
            )
        add_executable(${_test} ${_test}.cpp ${_test}.moc)
        qt4_generate_moc(${_test}.cpp ${_test}.moc)
        target_link_libraries(${_test}
            ${QT_QTTEST_LIBRARIES}
            unity-2d-private
            unity-2d-private-qml
            )
        set(_test_list "${_test_list};${_test}")
    endforeach(_test)
endmacro(libunity_2d_tests)

libunity_2d_tests(
    gconnectortest
    keyboardmodifiersmonitortest
    launchermenutest
    listaggregatormodeltest
    qsortfilterproxymodeltest
    focuspathtest
    pointerbarriertest
    )

target_link_libraries(pointerbarriertest ${X11_XTest_LIB})
    
# unity2dtrtest - FIXME
#add_test(NAME unity2dtrtest_check
#    COMMAND /bin/sh unity2dtrtest.sh
#    )
add_executable(unity2dtrtest
    unity2dtrtest.cpp unity2dtrtest.moc
    )
qt4_generate_moc(unity2dtrtest.cpp
    unity2dtrtest.moc
    )
target_link_libraries(unity2dtrtest
    ${QT_QTTEST_LIBRARIES}
    unity-2d-private
    unity-2d-private-qml
    )
configure_file(unity2dtrtest.sh.in
    unity2dtrtest.sh @ONLY)

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

#FIXME: rewrite this using dbus-test-runner to fake gnome-session messaging
#add_test(NAME gnomesessionclient_check
#    COMMAND /bin/sh gnomesessionclienttest.sh
#    )

