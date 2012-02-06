#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Albert Astals Cid <albert.astals@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
=end

require '../run-tests.rb' unless $INIT_COMPLETED
require 'xdo/xwindow'
require 'xdo/keyboard'
require 'xdo/mouse'
require 'tmpwindow'

############################# Test Suite #############################
context "Dash Tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-places'
    $SUT.execute_shell_command 'killall unity-2d-places'

    $SUT.execute_shell_command 'killall unity-2d-launcher'
    $SUT.execute_shell_command 'killall unity-2d-launcher'

    $SUT.execute_shell_command 'killall unity-2d-spread'
    $SUT.execute_shell_command 'killall unity-2d-spread'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end

  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @app_launcher = $SUT.run( :name => UNITY_2D_LAUNCHER, 
                              :arguments => "-testability", 
                              :sleeptime => 2 )

    @app_places = $SUT.run( :name => UNITY_2D_PLACES, 
                              :arguments => "-testability", 
                              :sleeptime => 2 )

    @app_spread = $SUT.run( :name => UNITY_2D_SPREAD, 
                            :arguments => "-testability", 
                            :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-spread'
    $SUT.execute_shell_command 'pkill -nf unity-2d-places'
    $SUT.execute_shell_command 'pkill -nf unity-2d-launcher'
  end

  #####################################################################################
  # Test casess

  # Test case objectives:
  #   * Check that Alt+F2 shows dash
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Press Alt+F2
  #   * Verify dash is showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Alt+F2 shows the Dash" do
    verify_not(0, 'There should not be a Dash declarative view on startup') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app_places.DashDeclarativeView()
    }
  end

  # Test case objectives:
  #   * Check that pressing the bfb shows the dash
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Click on the bfb
  #   * Verify dash is showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Pressing the bfb shows the Dash" do
    verify_not(0, 'There should not be a Dash declarative view on startup') {
      @app_places.DashDeclarativeView()
    }
    bfb = @app_launcher.LauncherList( :name => 'main' ).LauncherList( :isBfb => true );
    XDo::Mouse.move(0, 200, 0, true)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when mouse at left edge of screen' ) {
      @app_launcher.Unity2dPanel()['x_absolute'].to_i
    }
    bfb.move_mouse()
    bfb.tap()
    verify(TIMEOUT, 'There should be a Dash declarative view after activating the bfb') {
        @app_places.DashDeclarativeView()
    }
  end

  # Test case objectives:
  #   * Check that Super+s does not type s
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Press Alt+F2
  #   * Verify dash is showing
  #   * Verify there is no text in the search entry
  #   * Press Super+s
  #   * Verify there is no text in the search entry
  # Post-conditions
  #   * None
  # References
  #   * None
  # FIXME This does not work reliably because the spread clears the search entry
  #       and thus sometimes we ask for the contents too late
  xtest "Super+s does not type s" do
    verify_not(0, 'There should not be a Dash declarative view on startup') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app_places.DashDeclarativeView()
    }
    verify_equal("", TIMEOUT, 'There should be no text in the Search Entry') {
      @app_places.DashDeclarativeView().SearchEntry().QDeclarativeTextInput()['text']
    }
    XDo::Keyboard.super_s
    verify_not(0, "Text of the Search Entry should not be an 's'") {
      verify_equal( "s", 1 ) {
        @app_places.DashDeclarativeView().SearchEntry().QDeclarativeTextInput()['text']
      }
    }
  end

  # Test case objectives:
  #   * Check Super and Alt+F1 interaction
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Press Super
  #   * Verify dash is showing
  #   * Press Alt+F1
  #   * Verify dash is not showing
  #   * Verify launcher does not hide
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Super and Alt+F1 interaction" do
    xid = TmpWindow.open_window_at(10,100)
    verify_not(2, 'There should not be a Dash declarative view on startup') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.super
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_not(2, 'There should not be a Dash declarative view after pressing Alt+F1') {
      @app_places.DashDeclarativeView()
    }
    verify_not(0, 'Launcher should not hide after pressing Alt+F1') {
      verify_equal( -LAUNCHER_WIDTH, 2 ) {
        @app_launcher.Unity2dPanel()['x_absolute'].to_i
      }
    }
    xid.close!
  end

  # Test case objectives:
  #   * Check Super, Super and Alt+F1 interaction
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Verify terminal has focus
  #   * Press Super
  #   * Verify dash is showing
  #   * Press Super
  #   * Verify dash is not showing
  #   * Verify terminal has focus
  #   * Verify launcher is hidden
  #   * Press Alt+F1
  #   * Verify dash is not showing
  #   * Verify launcher shows
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Super, Super and Alt+F1 interaction" do
    XDo::Mouse.move(200, 200, 0, true)
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( xid.id, TIMEOUT, 'terminal should have focus after starting it' ) {
      XDo::XWindow.active_window
    }
    verify_not(2, 'There should not be a Dash declarative view on startup') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.super
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.super
    verify_not(2, 'There should not be a Dash declarative view after pressing Super again') {
      @app_places.DashDeclarativeView()
    }
    verify_equal( xid.id, TIMEOUT, 'terminal should have focus after toggling the dash' ) {
      XDo::XWindow.active_window
    }
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher should be hiding after toggling the dash' ) {
      @app_launcher.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_not(2, 'There should not be a Dash declarative view after pressing Alt+F1') {
      @app_places.DashDeclarativeView()
    }
    verify_equal( 0, TIMEOUT, 'Launcher should be showing after pressing Alt+F1' ) {
      @app_launcher.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  # Test case objectives:
  #   * Check navigation left from dash is disabled
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Invoke dash
  #   * Focus dash contents, press left
  #   * Check that focus is still on the first lens bar entry
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Check navigation left from dash is disabled" do
    XDo::Keyboard.super
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app_places.DashDeclarativeView()
    }
    loader = ""
    verify(TIMEOUT, 'Could not find the DashLoader') {
      loader = @app_places.QDeclarativeLoader( { :objectName => "pageLoader" } )
    }
    XDo::Keyboard.down
    verify_equal("true", TIMEOUT, 'Dash loader doesn\'t have focus') {
        loader['activeFocus']
    }
    XDo::Keyboard.left
    verify_not(0, 'Dash loader lost focus after pressing left') {
        verify_equal("false", 2) {
            loader['activeFocus']
        }
    }
  end

  # Test case objectives:
  #   * Check focus goes to dash on Super when launcher menu is open
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Invoke launcher with Alt+F1
  #   * Check the launcher is shown
  #   * Show the menu with Right arrow
  #   * Check the menu is shown
  #   * Invoke the dash with Super
  #   * Check the dash is shown
  #   * Type "as"
  #   * Check the dash search contains "as"
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Check focus goes to dash on Super when launcher menu is open" do
    XDo::Keyboard.alt_F1
    verify_equal( 0, TIMEOUT, 'Launcher hiding when Alt+F1 pressed' ) {
      @app_launcher.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Keyboard.right
    verify(TIMEOUT, 'There should be an unfolded menu after pressing Right') {
      @app_launcher.LauncherContextualMenu( :folded => false );
    }
    XDo::Keyboard.super
    verify(TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app_places.DashDeclarativeView()
    }
    XDo::Keyboard.a
    XDo::Keyboard.s
    verify_equal( "as", TIMEOUT, 'Text in the search field should be "as"' ) {
      @app_places.DashDeclarativeView().SearchEntry().QDeclarativeTextInput()['text']
    }
  end
end
