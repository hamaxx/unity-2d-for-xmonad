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
context "Launcher Autohide and Show Tests" do
  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-places > /dev/null 2>&1'
    system 'killall unity-2d-places > /dev/null 2>&1'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end

  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    # Make sure the launcher is running
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'unity-2d-launcher &'

    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")
    @app = @sut.run( :name => UNITY_2D_PLACES, 
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-places"
  end

  #####################################################################################
  # Test casess

  # Test case objectives:
  #   * Check the Launcher position on Empty desktop
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify Launcher is #{WIDTH} pixels wide
  #   * Verify Launcher showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Position with Empty Desktop" do
    # check width before proceeding
    verify_not(0, 'There should not be a Dash declarative view after pressing Alt+F2') {
      @app.DashDeclarativeView()
    }
    XDo::Keyboard.alt_F2 #Must use uppercase F to indicate function keys
    verify(0, 'There should be a Dash declarative view after pressing Alt+F2') {
        @app.DashDeclarativeView()
    }
  end
end
