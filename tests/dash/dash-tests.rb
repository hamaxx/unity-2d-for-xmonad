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
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'

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
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability",
                     :sleeptime => 2 )

    @app_spread = $SUT.run( :name => UNITY_2D_SPREAD, 
                            :arguments => "-testability", 
                            :sleeptime => 2 )
    # Move mouse out of way
    XDo::Mouse.move(200, 200, 0, true)
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-spread'
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  # Test case objectives:
  #   * Check that Alt+F4 hides dash
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Press Alt+F2
  #   * Verify dash is showing
  #   * Press Alt+F4
  #   * Verify dash is not showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Alt+F2, Alt+F4 hides dash" do
    verify_equal("false", TIMEOUT, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F2
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F4
    verify_equal("false", TIMEOUT, 'There should not be a Dash declarative view after pressing Escape') {
      @app.Dash()['active']
    }
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
    verify_equal("false", 0, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
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
    verify_equal("false", 0, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    bfb = @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true );
    XDo::Mouse.move(0, 200, 0, true)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when mouse at left edge of screen' ) {
      @app.Launcher()['x_absolute'].to_i
    }
    bfb.move_mouse()
    bfb.tap()
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after activating the bfb') {
        @app.Dash()['active']
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
    verify_equal("false", 0, 'Dash should be inactive on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify_equal("true", TIMEOUT, 'Dash should be active after pressing Alt+F2') {
      @app.Dash()['active']
    }
    verify_equal("", TIMEOUT, 'There should be no text in the Search Entry') {
      @app.Dash().SearchEntry().QDeclarativeTextInput()['text']
    }
    XDo::Keyboard.super_s
    verify_not(0, "Text of the Search Entry should not be an 's'") {
      verify_equal( "s", 1 ) {
        @app.Dash().SearchEntry().QDeclarativeTextInput()['text']
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
    verify_equal("false", 2, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.super
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_equal("false", 0, 'There should not be a Dash declarative view after pressing Alt+F1') {
      @app.Dash()['active']
    }
    verify_not(0, 'Launcher should not hide after pressing Alt+F1') {
      verify_equal( -LAUNCHER_WIDTH, 2 ) {
        @app.Launcher()['x_absolute'].to_i
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
    xid = TmpWindow.open_window_at(10,100)
    hide_mode = $SUT.execute_shell_command('gsettings get com.canonical.Unity2d.Launcher hide-mode').to_i
    verify_equal( xid.id, TIMEOUT, 'terminal should have focus after starting it' ) {
      XDo::XWindow.active_window
    }
    verify_equal("false", 2, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.super
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app.Dash()['active']
    }
    XDo::Keyboard.super
    verify_equal("false", 2, 'There should not be a Dash declarative view after pressing Super again') {
      @app.Dash()['active']
    }
    verify_equal( xid.id, TIMEOUT, 'terminal should have focus after toggling the dash' ) {
      XDo::XWindow.active_window
    }
    verify_equal((hide_mode == 0) ? 0 : -LAUNCHER_WIDTH, TIMEOUT, 
                 'Launcher position wrong after toggling the dash' ) {
      @app.Launcher()['x_absolute'].to_i
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_equal("false", 2, 'There should not be a Dash declarative view after pressing Alt+F1') {
      @app.Dash()['active']
    }
    verify_equal( 0, TIMEOUT, 'Launcher should be showing after pressing Alt+F1' ) {
      @app.Launcher()['x_absolute'].to_i
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
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app.Dash()['active']
    }
    loader = ""
    verify(TIMEOUT, 'Could not find the DashLoader') {
      loader = @app.QDeclarativeLoader( { :objectName => "pageLoader" } )
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
      @app.Launcher()['x_absolute'].to_i
    }
    XDo::Keyboard.right
    verify(TIMEOUT, 'There should be an unfolded menu after pressing Right') {
      @app.LauncherContextualMenu( :folded => false );
    }
    XDo::Keyboard.super
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app.Dash()['active']
    }
    XDo::Keyboard.a
    XDo::Keyboard.s
    verify_equal( "as", TIMEOUT, 'Text in the search field should be "as"' ) {
      @app.Dash().SearchEntry().QDeclarativeTextInput()['text']
    }
  end

  # Test case objectives:
  #   * Check that Alt+F2, Esc, Alt+F2 shows dash
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Press Alt+F2
  #   * Verify dash is showing
  #   * Press Esc
  #   * Verify dash is not showing
  #   * Press Alt+F2
  #   * Verify dash is showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Alt+F2, Esc, Alt+F2 shows dash" do
    verify_equal("false", 2, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
    XDo::Keyboard.escape
    verify_equal("false", 2, 'There should not be a Dash declarative view after pressing Escape') {
      @app.Dash()['active']
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
  end

  # Test case objectives:
  #   * Check navigation to and in lens bar
  # Pre-conditions
  # Test steps
  #   * Invoke dash
  #   * Go down 7 times (twice for each category in the Home lens + down to lens bar)
  #   * Verify that the lens bar has focus
  #   * Verify that the first lens button has focus
  #   * Go right
  #   * Verify that the second lens button has focus
  #   * Go right
  #   * Verify that the third lens button has focus
  #   * Go left twice
  #   * Verify that the first lens button has focus
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Check navigation down from dash leads to lens bar" do
    XDo::Keyboard.super
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Super') {
      @app.Dash()['active']
    }
    for i in 1..7 do
      XDo::Keyboard.down
    end
    verify_equal("true", TIMEOUT, 'Lens bar doesn\'t have focus') {
        @app.LensBar()['activeFocus']
    }

    buttons = @app.LensBar().children( { :type => "LensButton" } )

    verify_equal("true", TIMEOUT, 'First lens button doesn\'t have focus') {
        buttons[0]['activeFocus']
    }

    XDo::Keyboard.right
    verify_equal("true", TIMEOUT, 'Second lens button doesn\'t have focus') {
        buttons[1]['activeFocus']
    }

    XDo::Keyboard.right
    verify_equal("true", TIMEOUT, 'Third lens button doesn\'t have focus') {
        buttons[2]['activeFocus']
    }

    XDo::Keyboard.left
    XDo::Keyboard.left
    verify_equal("true", TIMEOUT, 'First lens button doesn\'t have focus') {
        buttons[0]['activeFocus']
    }

    XDo::Keyboard.up
    verify_equal("true", TIMEOUT, 'Dash contents don\'t have focus') {
        @app.QDeclarativeLoader( { :objectName => "pageLoader" } )['activeFocus']
    }
  end
end
