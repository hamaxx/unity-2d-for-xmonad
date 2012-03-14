#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Pawel Stolowski <pawel.stolowski@canonical.com>
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
require 'tmpwindow'
require 'launchercli'

############################# Test Suite #############################
context "Sizing tests" do
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

    # Make certain application is ready for testing
    verify{ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check that progress bar in Launcher item is correctly updated for progress values 0-100%.
  # Pre-conditions
  #   * 'Home Folder' item (nautilus) is present in the launcher.
  # Test steps
  #   * Report progress = 0% from nautilus, verify progress bar width is 0.
  #   * Report progress = 50% from nautilus, verify progress bar width is 1/2 of normal width
  #   * Report progress = 100% from nautilus, verify progress bar width is at full.
  #   * Hide progress bar of nautilus.
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Valid progress bar values" do
    lcli = LauncherItemClient.new('nautilus-home.desktop')
    lcli.set_progress(0.0)
   
    progressTopImg = @app.Launcher().QDeclarativeFocusScope().LauncherList(:name=>"main").QDeclarativeItem(:name=>"Home Folder").QDeclarativeImage(:name=>"progressBar")
    progressImg = progressTopImg.QDeclarativeImage()

    verify_equal( '', TIMEOUT, "Progress bar is visible" ) {
        progressTopImg['state']
    }

    verify_equal( 0, TIMEOUT, "Progress bar at 0%" ) {
        progressImg['width'].to_i
    }

    lcli.set_progress(0.5)
    verify_equal( progressImg['implicitWidth'].to_i / 2, TIMEOUT, "Progress bar at 50%" ) {
        progressImg['width'].to_i
    }

    lcli.set_progress(1.0)
    verify_equal( progressImg['implicitWidth'].to_i, TIMEOUT, "Progress bar at 100%" ) {
        progressImg['width'].to_i
    }

    lcli.hide_progress()
    verify_equal( 'hidden', TIMEOUT, "Progress bar is not visible" ) {
        progressTopImg['state']
    }
  end
 
  # Test case objectives:
  #   * Check that progress bar in Launcher item is correctly updated for invalid values (outside of 0-100% range).
  # Pre-conditions
  #   * 'Home Folder' item (nautilus) is present in the launcher.
  # Test steps
  #   * Report progress = -1 from nautilus, verify progress bar width is 0.
  #   * Report progress = 200% from nautilus, verify progress bar width is at full.
  #   * Hide progress bar of nautilus.
  # Post-conditions
  #   * None
  # References
  #   * None

  test "Invalid progress bar values" do
    lcli = LauncherItemClient.new('nautilus-home.desktop')
    lcli.set_progress(-1.0)

    progressTopImg = @app.Launcher().QDeclarativeFocusScope().LauncherList(:name=>"main").QDeclarativeItem(:name=>"Home Folder").QDeclarativeImage(:name=>"progressBar")
    progressImg = progressTopImg.QDeclarativeImage()
   
    verify_equal( 0, TIMEOUT, "Attempt to set negative progress value; progress set to 0%" ) {
        progressImg['width'].to_i
    }

    lcli.set_progress(2.0)
    verify_equal( progressImg['implicitWidth'].to_i, TIMEOUT, "Attempt to set big progress value; progress set to 100%" ) {
        progressImg['width'].to_i
    }

    lcli.hide_progress()
    verify_equal( 'hidden', TIMEOUT, "Progress bar is not visible" ) {
        progressTopImg['state']
    }
  end

end
