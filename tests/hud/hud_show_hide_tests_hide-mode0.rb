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
context "HUD Show and Hide tests" do
  hide_mode = 0

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'

    # Save current hide-mode
    hide_mode = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Launcher hide-mode'
    # Set hide-mode to 0 (fixed)
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 0'
  end

  # Run once at the end of this test suite
  shutdown do
    # Restore hide-mode to original setting
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode ' + hide_mode
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
    TmpWindow.close_all_windows
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
  #   * Check the Launcher does not hide when the HUD opens
  # Pre-conditions
  #   * None
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check Launcher remains on screen
  # Post-conditions
  #   * None
  # References
  #   * None
  test "HUD does not make Launcher hide" do
    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    
    XDo::Keyboard.alt
    verify_equal( 0, TIMEOUT, 'Launcher hid when HUD activated, should be visible' ) {
      @app.Launcher()['x_absolute'].to_i
    }
  end

  # Test case objectives:
  #   * Check the Launcher's BFB icon replaced when the HUD opens
  # Pre-conditions
  #   * Terminal window open & focused
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Check Launcher's BFB icon changes to a squircle with the terminal icon inside
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher's BFB icon replaced when HUD opens" do
    TmpWindow.open_window_at(100, 100)

    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    
    XDo::Keyboard.alt
    verify_equal('image://icons/utilities-terminal', TIMEOUT, 'BFB icon should have changed to Terminal icon' ) {
      @app.LauncherList( :name => 'main' ) \
          .LauncherList( :isBfb => true ) \
          .QDeclarativeImage( :name => 'icon' )['source']
    }
  end

  # Test case objectives:
  #   * Check the Launcher's BFB icon replaced by icon for first resuls (not necessarily the active
  #     application icon) when the HUD opens
  # Pre-conditions
  #   * Terminal window open & focused
  # Test steps
  #   * Check HUD closed
  #   * Tap the Alt key
  #   * Type "bluetooth"   #assuming as this is a trademark this string is not localised in menus
  #   * Check Launcher's BFB icon is the bluetooth icon
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher's BFB icon replaced when HUD opens" do
    TmpWindow.open_window_at(100, 100)

    verify_not(1, 'HUD should be hidden at startup') {
      @app.Hud()
    }
    
    XDo::Keyboard.alt
    XDo::Keyboard.simulate 'bluetooth'
    verify(TIMEOUT, 'BFB icon should have changed to Bluetooth icon' ) {
      icon = @app.LauncherList( :name => 'main' ) \
                 .LauncherList( :isBfb => true ) \
                 .QDeclarativeImage( :name => 'icon' )['source']
      icon == 'image://icons/bluetooth-active' || icon == 'image://icons/bluetooth-disabled' \
            || icon == 'image://icons/bluetooth-paired'
    }
  end
end
