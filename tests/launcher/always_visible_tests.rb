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
context "Launcher Always Visible Behaviour Tests" do
  hide_mode = 0

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
    #Ensure mouse out of the way
    XDo::Mouse.move(200,200,10,true)

    hide_mode = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Launcher hide-mode'

    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability", 
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode ' + hide_mode
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check the Launcher pushes the windows not to overlap with it
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Set the visibility behaviour to intellihide
  #   * Open a window that will overlap with the launcher
  #   * Set the visibility behaviour to always visible
  #   * Check the window was moved
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher pushes windows" do
      $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 2'
      xid = TmpWindow.open_window_at(LAUNCHER_WIDTH / 2, 10)
      $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 0'
      verify_equal( LAUNCHER_WIDTH + 1, TIMEOUT, 'Window was not pushed by setting Launcher to always visible' ) {
        xid.position[0]
      }
      xid.close!
  end

  # Test case objectives:
  #   * Check the Launcher does not push itself
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Set the visibility behaviour to intellihide
  #   * Verify launcher x is 0
  #   * Set the visibility behaviour to always visible
  #   * Verify launcher x is not different than 0 for 1 continued second
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher does not push itself" do
      $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 2'
      verify_equal( 0, TIMEOUT, 'Launcher hiding on empty desktop, should be visible' ) {
        @app.Launcher()['x_absolute'].to_i
      }
      $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 0'
      verify_not(0, 'Launcher x should be 0 after setting the always visible mode') {
        verify_true( 1 ) {
          @app.Launcher()['x_absolute'].to_i != 0
        }
      }
  end
end
