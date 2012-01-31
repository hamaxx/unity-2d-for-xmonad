#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Gerry Boland <gerry.boland@canonical.com>
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
require $library_path + '/../../launcher/autohide_show_tests_common.rb'

############################# Test Suite #############################
context "Launcher Autohide and Show Tests on RTL" do
  launcher_favorites = ""

  def verify_launcher_visible(timeout, message = '')
    verify_equal( XDo::XWindow.display_geometry[0] - LAUNCHER_WIDTH, timeout, message ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
  end

  def verify_launcher_hidden(timeout, message = '')
    verify_equal( XDo::XWindow.display_geometry[0], timeout, message ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
  end

  def open_overlapping_window()
    xid = TmpWindow.open_window_at(100, 100)
    xid.move(XDo::XWindow.display_geometry[0] - xid.size[0] - 10 , 100)
    return xid
  end

  def move_mouse_to_screen_edge()
    XDo::Mouse.move(XDo::XWindow.display_geometry[0], 200, 0, true)
  end

  def move_mouse_to_launcher_inner_border()
    XDo::Mouse.move(XDo::XWindow.display_geometry[0] - LAUNCHER_WIDTH, 200)
  end

  def move_mouse_to_just_outside_launcher()
    XDo::Mouse.move(XDo::XWindow.display_geometry[0] - LAUNCHER_WIDTH - 1, 200)
  end

  def move_window_just_overlapping(xid)
    xid.move(XDo::XWindow.display_geometry[0] - xid.size[0] - LAUNCHER_WIDTH - 1 , 100)
  end

  def move_window_just_not_overlapping(xid)
    xid.move(XDo::XWindow.display_geometry[0] - xid.size[0]- LAUNCHER_WIDTH - 2, 100)
  end

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-launcher'
    $SUT.execute_shell_command 'killall unity-2d-launcher'

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

    launcher_favorites = $SUT.execute_shell_command 'gsettings get com.canonical.Unity.Launcher favorites'

    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_LAUNCHER,
                     :arguments => "-testability,-reverse",
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.Unity2dPanel() }
  end

  # Run after each test case completes
  teardown do
    TmpWindow.close_all_windows
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-launcher'
    $SUT.execute_shell_command "gsettings set com.canonical.Unity.Launcher favorites \"" + launcher_favorites + "\""
  end

  #####################################################################################
  # Test cases

  test "Position with Empty Desktop" do
    test_position_with_empty_desktop()
  end

  test "Position with Window not in the way" do
    test_position_width_window_not_in_the_way()
  end

  test "Position with Window in the way" do
    test_position_with_window_in_the_way()
  end

  test "Move window positioning to check launcher action" do
    test_move_window_positioning_to_check_launcher_action()
  end

  xtest "Reveal hidden Launcher with mouse" do
    test_reveal_hidden_launcher_with_mouse()
  end

  test "Hold Super key down to reveal launcher and shortcut keys" do
    test_hold_super_launcher_shortcuts()
  end

  test "Press Alt+F1 to focus Launcher" do
    test_alt_f1_focus_launcher()
  end

  test "Press Alt+F1 to focus/unfocus Launcher" do
    test_alt_f1_focus_unfocus_launcher()
  end

  test "Launcher visible on show-desktop" do
    test_launcher_visible_show_desktop()
  end

  xtest "Launcher hide delay on tile removal" do
    test_launcher_hide_delay_on_tile_removal()
  end

  xtest "Launcher visible after toggling dash" do
    test_launcher_visible_after_toggling_dash()
  end

  test "Launcher does not hide on Esc after Alt+F1 with overlapping window" do
    test_launcher_does_not_hide_on_esc_after_alt_f1_with_overlapping_window()
  end

end
