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

############################# Test Suite #############################
context "HUD Show and Hide tests" do

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'
  end

  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    #Ensure mouse out of the way
    XDo::Mouse.move(500,500,10,true)

    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability", 
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.LauncherLoader() }
  end

  # Run after each test case completes
  teardown do
    #Need to kill Shell as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check the HUD displays with the Alt key
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check HUD open
  #   * Tap Alt key
  #   * Check HUD closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD reveals and hides with Alt key tap" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }

    XDo::Keyboard.alt
    verify_not(1, 'HUD should be hidden, as the Alt key should dismiss it') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Check Escape key closes HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check HUD open
  #   * Tap Escape key
  #   * Check HUD closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD hides with Escape key" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }

    XDo::Keyboard.escape
    verify_not(1, 'HUD should be hidden, as the Escape key should dismiss it') {
      @app.Hud()
    }
  end
 
  # Test case objectives:
  #   * Check Escape key clears search query first, then closes HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check HUD open
  #   * Type search string
  #   * Verify that search string appears in the HUD
  #   * Tap Escape key
  #   * Verify that search string was cleared
  #   * Tap Escape key
  #   * Check that HUD was closed
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/966121
  test "Esc clears HUD search query, then hides HUD" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }

    # type search string
    XDo::Keyboard.a
    XDo::Keyboard.b
    XDo::Keyboard.c

    verify_equal('abc', TIMEOUT, 'HUD search text should be "abc"') {
        @app.Hud().SearchEntry()['searchQuery']
    }

    XDo::Keyboard.escape
    verify_equal('true', TIMEOUT, 'HUD should still be visible') {
      @app.Hud()['active']
    }

    verify_equal('', TIMEOUT, 'HUD search text should be empty') {
        @app.Hud().SearchEntry()['searchQuery']
    }

    XDo::Keyboard.escape
    verify_not(1, 'HUD should be hidden, as the Escape key should dismiss it') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Check Alt+F4 key closes HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check HUD open
  #   * Tap Alt+F4 key
  #   * Check HUD closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD hides with ALT+F4 key" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }

    XDo::Keyboard.alt_F4
    verify_not(1, 'HUD should be hidden, as the ALT+F4 key should dismiss it') {
      @app.Hud()
    }
  end
  
  # Test case objectives:
  #   * Check outside mouse-click closes HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check HUD open
  #   * Move mouse to position outside HUD and click
  #   * Check HUD closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD hides when mouse clicks outside it" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }

    XDo::Mouse.move(400, 400, 10, true)
    XDo::Mouse.click
    verify_not(1, 'HUD should be hidden, mouse clicked outside it') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Check the Launcher hides when the HUD opens
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check Launcher hides
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD makes Launcher hide" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    
    XDo::Keyboard.alt
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible when HUD activated, should be hidden' ) {
      @app.Launcher()['x_absolute'].to_i
    }
  end
  
  # Test case objectives:
  #   * Check long Alt key hold doesn't open HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Hold down the Alt key
  #   * Check HUD stays closed
  #   * Release Alt key after 1 second
  #   * Check HUD still closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Long Alt key press does not open HUD" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.key_down('Alt')
    sleep 0.2
    verify_not(1, 'HUD should be hidden, as the Alt key is held down') {
      @app.Hud()
    }

    XDo::Keyboard.key_up('Alt')
    verify_not(1, 'HUD should be hidden, as the long Alt key press should not reveal it') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Check Alt+key tap doesn't open HUD
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap Alt+f
  #   * Check HUD stays closed
  #   * Tap Ctrl+Alt
  #   * Check HUD stays closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Alt+key tap doesn't open HUD" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }

    XDo::Keyboard.char('alt+f')
    sleep 0.1
    verify_not(1, 'HUD should be hidden, as the Alt key was pressed with another key') {
      @app.Hud()
    }
    XDo::Keyboard.escape
    XDo::Keyboard.char('alt+ctrl')
    sleep 0.1
    verify_not(1, 'HUD should be hidden, as the Alt key was pressed with Ctrl key') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Hold Alt key, check Super key tap doesn't open Dash
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap Alt+f
  #   * Check HUD stays closed
  #   * Tap Ctrl+Alt
  #   * Check HUD stays closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Hold Alt key, check Super key tap doesn't open Dash" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    verify_equal('false', TIMEOUT, 'Dash should be hidden at startup') {
      @app.Dash()['active']
    }

    XDo::Keyboard.key_down('Alt')
    sleep 0.2
    verify_not(1, 'HUD should be hidden, as the Alt key is held down') {
      @app.Hud()
    }

    XDo::Keyboard.super
    sleep 0.1
    verify_equal('false', TIMEOUT, 'Dash should not be shown by Super tap while Alt held') {
      @app.Dash()['active']
    }

    XDo::Keyboard.key_up('Alt')
    verify_not(1, 'HUD should be hidden, as the Alt key held & Super key only tapped') {
      @app.Hud()
    }
  end

  # Test case objectives:
  #   * Check opening HUD closes Dash
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD & Dash closed
  #   * Tap Super key to open Dash
  #   * Check Dash open
  #   * Check HUD closed
  #   * Tap Alt to open HUD
  #   * Check Dash closed
  #   * Check HUD open
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Opening HUD closes Dash" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    verify_equal('false', TIMEOUT, 'Dash should be hidden at startup') {
      @app.Dash()['active']
    }

    XDo::Keyboard.super
    verify_equal('true', TIMEOUT, 'Dash should be visible after pressing Super') {
      @app.Dash()['active']
    }
    verify_not(1, 'HUD should not appear when Dash summoned') {
      @app.Hud()
    }

    XDo::Keyboard.alt
    verify_equal('false', TIMEOUT, 'Dash should hide as HUD summoned') {
      @app.Dash()['active']
    }
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }
  end

  # Test case objectives:
  #   * Check opening Dash closes Hud
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD & Dash closed
  #   * Tap Alt to open HUD
  #   * Check HUD open
  #   * Check Dash closed
  #   * Tap Super key to open Dash
  #   * Check Dash open
  #   * Check HUD closed
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Opening Dash closes HUD" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    verify_equal('false', TIMEOUT, 'Dash should be hidden at startup') {
      @app.Dash()['active']
    }

    XDo::Keyboard.alt
    verify_equal('true', TIMEOUT, 'HUD should be visible, as the Alt key was tapped') {
      @app.Hud()['active']
    }
    verify_equal('false', TIMEOUT, 'Dash should be hidden as HUD summoned') {
      @app.Dash()['active']
    }

    XDo::Keyboard.super
    verify_equal('true', TIMEOUT, 'Dash should be visible after pressing Super') {
      @app.Dash()['active']
    }
    verify_not(1, 'HUD should hide when Dash summoned') {
      @app.Hud()
    }
  end
end
