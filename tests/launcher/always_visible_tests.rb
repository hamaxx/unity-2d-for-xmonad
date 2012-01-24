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
  WIDTH = 65 #launcher bar width
  hide_mode = 0

  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'killall unity-2d-launcher > /dev/null 2>&1'

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

    hide_mode = `gsettings get com.canonical.Unity2d.Launcher hide-mode`

    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")
    @app = @sut.run( :name => UNITY_2D_LAUNCHER,
                     :arguments => "-testability", 
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.Unity2dPanel() }
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-launcher"
    system "gsettings set com.canonical.Unity2d.Launcher hide-mode " + hide_mode
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
      system "gsettings set com.canonical.Unity2d.Launcher hide-mode 2"
      xid = TmpWindow.open_window_at(WIDTH / 2, 10)
      system "gsettings set com.canonical.Unity2d.Launcher hide-mode 0"
      verify_equal( WIDTH + 1, TIMEOUT, 'Window was not pushed by setting Launcher to always visible' ) {
        xid.position[0]
      }
      xid.close!
  end
end
