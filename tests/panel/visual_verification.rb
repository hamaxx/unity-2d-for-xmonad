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
require 'xdo/keyboard'
require 'xdo/mouse'
require 'xdo/xwindow'

############################# Test Suite #############################
context "Panel visual verification tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'
  old_value = ""

  def dash_always_fullscreen
    out = XDo::XWindow.display_geometry()
    width = out[0]
    height = out[1]
    return width < DASH_MIN_SCREEN_WIDTH && height < DASH_MIN_SCREEN_HEIGHT
  end

  # Run once at the beginning of this test suite
  startup do
    old_value = $SUT.execute_shell_command 'gsettings get com.canonical.Unity2d.Dash full-screen'
    $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Dash full-screen false'

    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-panel'

    $SUT.execute_shell_command 'killall unity-2d-launcher'
    $SUT.execute_shell_command 'killall unity-2d-launcher'
  end

  # Run once at the end of this test suite
  shutdown do
      $SUT.execute_shell_command 'gsettings set com.canonical.Unity2d.Dash full-screen ' + old_value
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_PANEL,
                     :arguments => "-testability",
                     :sleeptime => 2 )

    @launcher = $SUT.run( :name => UNITY_2D_LAUNCHER,
                       :arguments => "-testability",
                       :sleeptime => 2 )

  end

  # Run after each test case completes
  teardown do
    $SUT.execute_shell_command 'pkill -nf unity-2d-panel'
    $SUT.execute_shell_command 'pkill -nf unity-2d-launcher'
  end

  #####################################################################################
  # Test cases

  test "Visually compare dash buttons with reference" do
    XDo::Keyboard.simulate('{SUPER}')

    closeButton = @app.AppNameApplet().QAbstractButton( :name => 'AppNameApplet::CloseButton' )
    verify_true( TIMEOUT, 'close button not matching reference image' ) {
      closeButton.find_on_screen(pwd + 'verification/dash_close_button.png') != nil
    }

    minimizeButton = @app.AppNameApplet().QAbstractButton( :name => 'AppNameApplet::MinimizeButton' )
    verify_true( TIMEOUT, 'minimize button not matching reference image' ) {
      minimizeButton.find_on_screen(pwd + 'verification/dash_minimize_button.png') != nil
    }

    maximizeButton = @app.AppNameApplet().QAbstractButton( :name => 'AppNameApplet::MaximizeButton' )
    if dash_always_fullscreen
      verify_true( TIMEOUT, 'maximize button fullscreen not matching reference image' ) {
        maximizeButton.find_on_screen(pwd + 'verification/dash_maximize_button_fullscreen.png') != nil
      }
    else
      verify_true( TIMEOUT, 'maximize button not matching reference image' ) {
        maximizeButton.find_on_screen(pwd + 'verification/dash_maximize_button.png') != nil
      }

      maximizeButton.move_mouse()
      XDo::Mouse.down()
      verify_true( TIMEOUT, 'maximize button pressed not matching reference image' ) {
        maximizeButton.find_on_screen(pwd + 'verification/dash_maximize_button_pressed.png') != nil
      }
      XDo::Mouse.up()

      verify_true( TIMEOUT, 'maximize button fullscreen not matching reference image' ) {
        maximizeButton.find_on_screen(pwd + 'verification/dash_maximize_button_fullscreen.png') != nil
      }

      maximizeButton.move_mouse()
      XDo::Mouse.down()
      verify_true( TIMEOUT, 'maximize button fullscreen pressed not matching reference image' ) {
        maximizeButton.find_on_screen(pwd + 'verification/dash_maximize_button_fullscreen_pressed.png') != nil
      }
      XDo::Mouse.up()
    end

    closeButton.move_mouse()
    XDo::Mouse.down()
    verify_true( TIMEOUT, 'close button pressed not matching reference image' ) {
      closeButton.find_on_screen(pwd + 'verification/dash_close_button_pressed.png') != nil
    }
    XDo::Mouse.up()
  end
end
