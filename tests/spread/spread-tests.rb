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
context "Spread Tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-spread'
    $SUT.execute_shell_command 'killall unity-2d-spread'

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
    @app_shell = $SUT.run( :name => UNITY_2D_SHELL,
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
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
    $SUT.execute_shell_command 'pkill -nf unity-2d-spread'
    # Need to wait for the spread to really die, otherwise bad things happen
    # on the second test
    spread = $SUT.execute_shell_command 'bash -c "ps -A | grep unity-2d-spread"'
    while spread != ""
      sleep 0.1
      spread = $SUT.execute_shell_command 'bash -c "ps -A | grep unity-2d-spread"'
    end
  end

  #####################################################################################
  # Test casess

  # Test case objectives:
  #   * Check that Super+s shows the launcher and the spread
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify spread is not showing
  #   * Open application that overlaps with the launcher
  #   * Verify launcher is not showing
  #   * Press Super+s
  #   * Verify spread is showing
  #   * Verify launcher is showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Super+s shows the launcher and the spread" do
    hide_mode = $SUT.execute_shell_command('gsettings get com.canonical.Unity2d.Launcher hide-mode').to_i

    verify_not(2, 'There should not be a Spread declarative view on startup') {
      @app_spread.SpreadView()
    }
    xid = TmpWindow.open_window_at(10,100)

    verify_equal((hide_mode == 0) ? 0 :-LAUNCHER_WIDTH, TIMEOUT, 
                 'The launcher should not be visible with an overlapping window (unless Launcher in always-show mode)') {
      @app_shell.Launcher()['x_absolute'].to_i
    }
    XDo::Keyboard.super_s
    verify_equal("true", TIMEOUT, 'There should be a visible Spread declarative view after pressing Alt+F2') {
      @app_spread.SpreadView()['visible']
    }
    verify_equal(0, TIMEOUT, 'The launcher should be visible when spread is invoked') {
      @app_shell.Launcher()['x_absolute'].to_i
    }
    xid.close!
  end

  # Test case objectives:
  #   * Check that clicking in the spread item of the launcher toggles the spread
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify spread is not showing
  #   * Click in the spread item of the launcher
  #   * Verify spread is showing
  #   * Click in the spread item of the launcher
  #   * Verify spread is not showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Spread launcher item toggles the spread" do
    hide_mode = $SUT.execute_shell_command('gsettings get com.canonical.Unity2d.Launcher hide-mode')
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 0'

    verify_not(2, 'There should not be a Spread declarative view on startup') {
      @app_spread.SpreadView()
    }
    spreadItem = @app_shell.LauncherList( :name => 'main' ).LauncherList( :icon => "image://icons/unity-icon-theme/workspace-switcher" );
    spreadItem.move_mouse()
    XDo::Mouse.click

    verify_equal("true", TIMEOUT, 'There should be a visible Spread declarative view after clicking on the spread item') {
      @app_spread.SpreadView()['visible']
    }
    sleep 2 # Do not really need it, but otherwise the animation looks weird
    XDo::Mouse.click
    verify_not(2, 'There should not be a visible Spread declarative view after clicking again on the spread item') {
      @app_spread.SpreadView()
    }
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode ' + hide_mode
  end
end
