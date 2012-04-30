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
require 'xdo/keyboard'
require 'tmpwindow'

############################# Test Suite #############################
context "Panel visual verification tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-panel'

    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'
  end

  # Run once at the end of this test suite
  teardown do
    TmpWindow.close_all_windows
    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-shell'
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @panel = $SUT.run( :name => UNITY_2D_PANEL,
                     :arguments => "-testability",
                     :sleeptime => 2 )

    @launcher = $SUT.run( :name => UNITY_2D_SHELL,
                       :arguments => "-testability",
                       :sleeptime => 2 )

  end

  # Run after each test case completes
  teardown do
    $SUT.execute_shell_command 'pkill -nf unity-2d-panel'
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases
  #
  
  # Test case objectives:
  #  * Check that killing shell while dash is open resets panel
  # Pre-conditions
  #  * Desktop with no running apps
  # Test steps
  #   * Press Super
  #   * Verify dash is showing
  #   * kill shell process
  #   * Verify panel caption says 'Ubuntu Desktop'
  #   * Verify caption absolute x position is 0 (=no window buttons are displayed)
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/953168
  test "Killing shell while dash is active resets panel to sane state" do
    XDo::Keyboard.super

    verify_equal("true", TIMEOUT, 'There should be a Dash after pressing Super') {
      @launcher.Dash()['active']
    }
    
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'

    verify_equal('Ubuntu Desktop', TIMEOUT, 'Panel caption should be Ubuntu Desktop') {
        @panel.Unity2dPanel(:name=>'0').AppNameApplet().CroppedLabel()['text']
    }

    verify_equal(0, TIMEOUT, 'Panel label should start at left edge of screen, meaning no window buttons are displayed') {
        @panel.Unity2dPanel(:name=>'0').AppNameApplet().CroppedLabel()['x_absolute'].to_i
    }
  end

  # Test case objectives:
  #  * Check that killing shell while hud is open resets panel
  # Pre-conditions
  #  * Desktop with no running apps
  # Test steps
  #   * Press Alt
  #   * Verify hud is showing
  #   * kill shell process
  #   * Verify panel caption says 'Ubuntu Desktop'
  #   * Verify caption absolute x position is 0 (=no window buttons are displayed)
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/953168
  test "Killing shell while hud is active resets panel to sane state" do
    XDo::Keyboard.alt

    verify_equal("true", TIMEOUT, 'There should be a Hud after pressing Alt') {
      @launcher.Hud()['active']
    }
    
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'

    verify_equal('Ubuntu Desktop', TIMEOUT, 'Panel caption should be Ubuntu Desktop') {
        @panel.Unity2dPanel(:name=>'0').AppNameApplet().CroppedLabel()['text']
    }

    verify_equal(0, TIMEOUT, 'Panel label should start at left edge of screen, meaning no window buttons are displayed') {
        @panel.Unity2dPanel(:name=>'0').AppNameApplet().CroppedLabel()['x_absolute'].to_i
    }
  end

  # Test case objectives:
  #  * Check that closing the dash using the panel resets focus
  # Pre-conditions
  #  * Desktop with no running apps
  # Test steps
  #   * Open window
  #   * Verify window has focus
  #   * Open dash
  #   * Verify window does not have focus
  #   * Close dash from the panel [x] button
  #   * Verify window gets focus back
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/953168
  test "Focus returns to window on dash [x] press" do
    xid = TmpWindow.open_window_at(100,100)
    verify_true(TIMEOUT, 'terminal should have focus after starting it') {
      xid.id == XDo::XWindow.active_window
    }

    XDo::Keyboard.simulate('{SUPER}')
    verify_false(TIMEOUT, 'terminal has focus when it should be in the dash') {
      xid.id == XDo::XWindow.active_window
    }

    @panel.QAbstractButton( :name => 'AppNameApplet::CloseButton' ).move_mouse
    @panel.QAbstractButton( :name => 'AppNameApplet::CloseButton' ).tap

    verify_true(TIMEOUT, 'terminal does not have focus when it should') {
      xid.id == XDo::XWindow.active_window
    }

    xid.close!
  end

  # Test case objectives:
  #  * Check that closing the hud using the panel resets focus
  # Pre-conditions
  #  * Desktop with no running apps
  # Test steps
  #   * Open window
  #   * Verify window has focus
  #   * Open hud
  #   * Verify window does not have focus
  #   * Close hud from the panel [x] button
  #   * Verify window gets focus back
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/953168
  test "Focus returns to window on hud [x] press" do
    xid = TmpWindow.open_window_at(100,100)
    verify_true(TIMEOUT, 'terminal should have focus after starting it') {
      xid.id == XDo::XWindow.active_window
    }

    XDo::Keyboard.simulate('{ALT}')
    verify_false(TIMEOUT, 'terminal has focus when it should be in the hud') {
      xid.id == XDo::XWindow.active_window
    }

    @panel.QAbstractButton( :name => 'AppNameApplet::CloseButton' ).move_mouse
    @panel.QAbstractButton( :name => 'AppNameApplet::CloseButton' ).tap

    verify_true(TIMEOUT, 'terminal does not have focus when it should') {
      xid.id == XDo::XWindow.active_window
    }

    xid.close!
  end

end
