#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-spread'
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check Dash moving with Alt+F2
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify dash is not showing
  #   * Move mouse to the first screen
  #   * Press Alt+F2
  #   * Verify dash is showing in the first screen
  #   * Move mouse to the second screen
  #   * Press Alt+F2
  #   * Verify dash is showing in the second screen
  #   * Move mouse to the first screen
  #   * Press Alt+F2
  #   * Verify dash is showing in the first screen
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Check Dash moving with Alt+F2" do
    verify_equal("false", 0, 'There should not be a Dash declarative view on startup') {
      @app.Dash()['active']
    }

    XDo::Mouse.move(100, 200, 0, true)
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
    verify_equal(LAUNCHER_WIDTH, TIMEOUT, 'The Dash should be in the first screen') {
      @app.Dash()['x_absolute'].to_i
    }

    XDo::Mouse.move(XDo::XWindow.display_geometry()[0] + 100, 200, 0, true)
    XDo::Keyboard.alt_F2
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
    verify_equal(XDo::XWindow.display_geometry()[0] + LAUNCHER_WIDTH, TIMEOUT, 'The Dash should be in the second screen') {
      @app.Dash()['x_absolute'].to_i
    }
    XDo::Keyboard.a
    XDo::Keyboard.s
    verify_equal( "as", TIMEOUT, 'Text in the search field should be "as"' ) {
      @app.Dash().SearchEntry().QDeclarativeTextInput()['text']
    }

    XDo::Mouse.move(100, 200, 0, true)
    XDo::Keyboard.alt_F2
    verify_equal("true", TIMEOUT, 'There should be a Dash declarative view after pressing Alt+F2') {
      @app.Dash()['active']
    }
    verify_equal(LAUNCHER_WIDTH, TIMEOUT, 'The Dash should be in the first screen') {
      @app.Dash()['x_absolute'].to_i
    }
    XDo::Keyboard.a
    XDo::Keyboard.s
    verify_equal( "asas", TIMEOUT, 'Text in the search field should be "as"' ) {
      @app.Dash().SearchEntry().QDeclarativeTextInput()['text']
    }

  end
end
