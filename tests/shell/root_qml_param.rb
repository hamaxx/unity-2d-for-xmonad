#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
context "Shell root file param test" do
  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-shell > /dev/null 2>&1'
    system 'killall unity-2d-shell > /dev/null 2>&1'

  end

  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
  end

  # Run after each test case completes
  teardown do
    system "pkill -nf unity-2d-shell"
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check the app does not start with the -rootqml parameter but without path for the file
  # Pre-conditions
  #   * None
  # Test steps
  #   * Verify app does not start
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify app does not start without a rootqml parameter" do
    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")
    verify_not(0) { @app = @sut.run( :name => UNITY_2D_SHELL,
                                    :arguments => "-testability,-rootqml", 
                                    :sleeptime => 2 ) }
  end

  # Test case objectives:
  #   * Check the app loads the qml file specified in the -rootqml parameter
  # Pre-conditions
  #   * None
  # Test steps
  #   * Verify app starts
  #   * Verify the contents of the qml file are loaded
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify app starts with a valid rootqml parameter" do
    # Execute the application 
    testQmlFile = File.expand_path(File.dirname(__FILE__)) + "/rootqmlparamtest.qml"
    @sut = TDriver.sut(:Id => "sut_qt")
    @app = @sut.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability,-rootqml," + testQmlFile, 
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app }
    verify{ @app.QDeclarativeText( :text => 'This is rootqmlparamtest.qml' ) }
  end

end
