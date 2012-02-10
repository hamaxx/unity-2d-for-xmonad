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
context "Sizing tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'

    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-panel'

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

    # Make sure the panel is running
    @app_panel = $SUT.run( :name => UNITY_2D_PANEL, 
                           :arguments => "-testability", 
                           :sleeptime => 2 )

    # Make certain application is ready for testing
    verify{ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #Need to kill Launcher and Panel as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check the Launcher updates size and position on free desktop space change
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify Launcher y position is the panel height
  #   * Kill panel
  #   * Verify Launcher y position is zero
  #   * Verify Launcher height is old height + panel height
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher updates size and position on free desktop space change" do
    # check width before proceeding
    verify_equal( PANEL_HEIGHT, TIMEOUT, "Launcher is not just under the panel" ) {
      @app.Launcher()['y_absolute'].to_i
    }
    old_height = @app.Launcher()['height'].to_i
    $SUT.execute_shell_command 'pkill -nf unity-2d-panel'
    verify_equal( 0, TIMEOUT, "Launcher did not move to the top of the screen when killing the panel" ) {
      @app.Launcher()['y_absolute'].to_i
    }
    verify_equal( old_height + PANEL_HEIGHT, TIMEOUT, "Launcher did not grow vertically when killing the panel" ) {
      @app.Launcher()['height'].to_i
    }
  end
end
