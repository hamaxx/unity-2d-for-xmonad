#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Michał Sawicz <michal.sawicz@canonical.com>
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
require $library_path + '/../../launcher/menu_tests_common.rb'

############################# Test Suite #############################
context "Launcher Contextual Menu Tests" do

  def keyboard_tap_right()
    XDo::Keyboard.left
  end

  def keyboard_tap_left()
    XDo::Keyboard.right
  end

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
    # Ensure mouse out of the way
    XDo::Mouse.move(200,200,10,true)

    # Execute the application
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability,-reverse",
                     :sleeptime => 2)
    # Make certain application is ready for testing
    verify{ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    #@app.close
    #@app.close
    #Need to kill Shell as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases
  test "Display launcher item hint" do
    test_display_launcher_item_hint()
  end

  test "Display launcher menu after right click" do
    test_display_launcher_menu_after_right_click()
  end

  test "Display launcher menu with keyboard navigation" do
    test_display_launcher_menu_with_keyboard_navigation()
  end

  test "Close launcher menu when pressing Esc" do
    test_close_launcher_menu_when_pressing_esc()
  end

  test "Close launcher menu when navigating back to the launcher" do
    test_close_launcher_menu_when_navigating_back_to_the_launcher()
  end

  test "Verify launcher tile gets focus after dismissing the menu with Esc" do
    test_verify_launcher_tile_gets_focus_after_dismissing_the_menu_with_esc()
  end

  test "Verify launcher tile gets focus after dismissing the menu with keyboard navigation" do
    test_verify_launcher_tile_gets_focus_after_dismissing_the_menu_with_keyboard_navigation()
  end

  test "Verify dash search entry gets focus after dismissing the menu with Super" do
    test_verify_dash_search_entry_gets_focus_after_dismissing_the_menu_with_super()
  end
end
