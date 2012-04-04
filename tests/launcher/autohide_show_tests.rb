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
context "Launcher Autohide and Show Tests" do
  launcher_favorites = ""
  hide_mode = 0
  reveal_mode = false

  def verify_launcher_visible(timeout, message = '')
    verify_equal( 0, timeout, message ) {
      @app.Launcher()['x_absolute'].to_i
    }
  end

  def verify_launcher_hidden(timeout, message = '')
    verify_equal( -LAUNCHER_WIDTH, timeout, message ) {
      @app.Launcher()['x_absolute'].to_i
    }
  end

  def open_overlapping_window()
    xid = TmpWindow.open_window_at(10,100)
    return xid
  end

  def move_mouse_to_screen_edge()
    XDo::Mouse.move(0, 200, 0, true)
  end

  def move_mouse_to_screen_corner()
    XDo::Mouse.move(0, 0, 0, true)
  end

  def mouse_push_screen_edge
    (1..100).each do |i|
      $SUT.execute_shell_command 'xdotool mousemove_relative -- -10 0'
    end
  end

  def move_mouse_to_launcher_inner_border()
    XDo::Mouse.move(LAUNCHER_WIDTH-1,200)
  end

  def move_mouse_to_just_outside_launcher()
    XDo::Mouse.move(LAUNCHER_WIDTH,200)
  end

  def move_window_just_overlapping(xid)
    xid.move(LAUNCHER_WIDTH - 1 , 100)
  end

  def move_window_just_not_overlapping(xid)
    xid.move(LAUNCHER_WIDTH, 100)
  end

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'

    hide_mode = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Launcher hide-mode'
    reveal_mode = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Launcher reveal-mode'
    
    # Set hide mode to intellihide
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode 2'

    # Set reveal mode to side
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher reveal-mode 0'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end
  
  # Run once at the end of this test suite
  shutdown do
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher hide-mode ' + hide_mode
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Launcher reveal-mode ' + reveal_mode
  end

  # Run before each test case begins
  setup do
    #Ensure mouse out of the way
    XDo::Mouse.move(200,200,10,true)

    launcher_favorites = $SUT.execute_shell_command 'gsettings get com.canonical.Unity.Launcher favorites'

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

  test "Reveal hidden Launcher with mouse" do
    test_reveal_hidden_launcher_with_mouse()
  end

  test "Press Super key to toggle launcher" do
    test_press_super_to_toggle_launcher()
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

  test "Press Alt+F1, esc to focus/unfocus Launcher when dash is open" do
    test_alt_f1_esc_focus_unfocus_launcher_when_dash_open()
  end

  test "Press Alt+F1 to focus Launcher when dash is open, Alt+F1 to unfocus" do
    test_alt_f1_toggle_focus_launcher_when_dash_open()
  end

  test "Launcher visible on show-desktop" do
    test_launcher_visible_show_desktop()
  end

  test "Launcher hide delay on tile removal" do
    test_launcher_hide_delay_on_tile_removal()
  end

  test "Launcher visible after toggling dash" do
    test_launcher_visible_after_toggling_dash()
  end

  test "Launcher does not hide on Esc after Alt+F1 with overlapping window" do
    test_launcher_does_not_hide_on_esc_after_alt_f1_with_overlapping_window()
  end

  test "Launcher hides immediately on Esc after Super with overlapping window" do
    test_launcher_hides_immediatel_on_esc_after_super_with_overlapping_window()
  end

  test "Launcher does not hide on Esc after Super with overlapping window and mouse on BFB" do
    test_launcher_does_not_hide_on_esc_after_Super_with_overlapping_window_mouse_on_bfb()
  end
  
  test "Auto Hide: Launcher does not hide on Esc after Alt+F1 and mouse on BFB" do
    test_auto_hide_launcher_does_not_hide_on_esc_after_alt_f1_mouse_on_bfb()
  end

  test "Auto Hide: Launcher hide timer and Hud interaction" do
    test_auto_hide_launcher_hide_timer_and_hud_interaction()
  end

  test "Auto Hide: Launcher mouse move just after barrier trigger" do
    test_auto_hide_launcher_mouse_move_just_after_barrier_trigger()
  end

  test "Auto Hide: Launcher mouse corner reveal" do
    test_auto_hide_launcher_mouse_corner_reveal()
  end

end
