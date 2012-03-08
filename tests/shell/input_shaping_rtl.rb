#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
require 'timeout'
require 'tmpdir'
require 'tempfile'
require $library_path + '/../../shell/input_shaping_common.rb'

include TDriverVerify

############################# Test Suite #############################
context "Shell input shape tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'
  hide_mode = 0

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-panel'

    hide_mode = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Launcher hide-mode'
    
    # Set hide mode to intellihide
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 2'

    # Need panel running as position of shell depends on it
    @@panel = $SUT.run(:name => UNITY_2D_PANEL,
                       :arguments => "-testability" )

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end
  
  # Run once at the end of this test suite
  shutdown do
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode ' + hide_mode
  end

  # Run before each test case begins
  setup do
    #Ensure mouse out of the way
    XDo::Mouse.move(200,200,10,true)

    # Execute the application 
    @app = $SUT.run(:name => UNITY_2D_SHELL,
                    :arguments => "-testability,-reverse",
                    :sleeptime => 2)
    # Make certain application is ready for testing
    verify(10){ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  test "Shape of launcher alone" do
    test_shape_of_launcher_alone(true)
  end

  test "Shape of launcher and desktop mode dash" do
    test_shape_of_launcher_and_desktop_mode_dash(true)
  end

  test "Shape of launcher and fullscreen mode dash" do
    test_shape_of_launcher_and_fullscreen_mode_dash(true)
  end
end
