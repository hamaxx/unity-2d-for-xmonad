#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Ugo Riboni <ugo.riboni@canonical.com>
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
require 'timeout'
require 'tmpdir'
require 'tempfile'
include TDriverVerify

############################# Test Suite #############################
context "Dash fullscreen tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'

  DASH_FULLSCREEN_KEY = '/com/canonical/unity-2d/dash/full-screen'
  DASH_FORMFACTOR_KEY = '/com/canonical/unity-2d/form-factor'

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-panel'
    $SUT.execute_shell_command 'killall unity-2d-panel'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end
  
  # Run once at the end of this test suite
  shutdown do
     XDo::XWindow.toggle_minimize_all
  end

  # Run before each test case begins
  setup do
    @fullscreen_old = ($SUT.execute_shell_command "dconf read #{DASH_FULLSCREEN_KEY}").chop
    @formfactor_old = ($SUT.execute_shell_command "dconf read #{DASH_FORMFACTOR_KEY}").chop

    # Execute the application
    @panel = $SUT.run(:name => UNITY_2D_PANEL,
                    :arguments => "-testability",
                    :sleeptime => 2)
    @shell = $SUT.run(:name => UNITY_2D_SHELL,
                    :arguments => "-testability",
                    :sleeptime => 2)

    verify(10){ @panel.Unity2dPanel( :name => '0' ) }
    verify(10){ @shell.ShellDeclarativeView() }
  end

  # Run after each test case completes
  teardown do
    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} #{@fullscreen_old}"
    $SUT.execute_shell_command "dconf write #{DASH_FORMFACTOR_KEY} #{@formfactor_old}"

    $SUT.execute_shell_command "pkill -nf unity-2d-panel"
    $SUT.execute_shell_command "pkill -nf unity-2d-shell"
  end

  def dash_always_fullscreen
    out = XDo::XWindow.display_geometry()
    width = out[0]
    height = out[1]
    return width < DASH_MIN_SCREEN_WIDTH && height < DASH_MIN_SCREEN_HEIGHT
  end

  #####################################################################################
  # Test cases

  test "Dash fullscreens on dconf key change" do
    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} false"
    $SUT.execute_shell_command "dconf write #{DASH_FORMFACTOR_KEY} 'desktop'"
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @shell.ShellDeclarativeView()['dashActive']
    }

    expected = dash_always_fullscreen ? 'FullScreenMode' : 'DesktopMode'
    verify_equal(expected, TIMEOUT, 'Dash is in the wrong fullscreen state') {
        @shell.ShellDeclarativeView()['dashMode']
    }

    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} true"
    sleep 1

    verify_equal('FullScreenMode', TIMEOUT, 'Dash is not fullscreen but should be') {
        @shell.ShellDeclarativeView()['dashMode']
    }
  end

  # FIXME: this test is temporarily disabled until we add back the panel buttons for the dash in
  # the shell branch.
  xtest "Dash reacts correctly to panel buttons" do
    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} false"
    $SUT.execute_shell_command "dconf write #{DASH_FORMFACTOR_KEY} 'desktop'"
    XDo::Keyboard.super
    sleep 1
    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @shell.ShellDeclarativeView()['dashActive']
    }

    maxbutton = nil
    verify(TIMEOUT, 'The "maximize" button did not appear when the dash was visible' ) {
        maxbutton = @panel.AppNameApplet().children( :type => 'QAbstractButton' )[2]
    }

    maxbutton.tap if maxbutton
    sleep 1
    verify_equal('FullScreenMode', TIMEOUT, 'Dash should be fullsceen, but it is not' ) {
        @shell.ShellDeclarativeView()['dashMode']
    }

    # When always fullscreen tapping the max button does nothing, so the key should remain set to
    # false.
    expected = dash_always_fullscreen ? 'false' : 'true'
    verify_equal(expected, TIMEOUT, 'Dash fullscreen key has the wrong value after maximize') {
        ($SUT.execute_shell_command "dconf read #{DASH_FULLSCREEN_KEY}").chop
    }

    maxbutton.tap if maxbutton
    sleep 1
    expected = dash_always_fullscreen ? 'FullScreenMode' : 'DesktopMode'
    verify_equal(expected, TIMEOUT, 'Dash is in the wrong fullscreen state' ) {
        @shell.ShellDeclarativeView()['dashMode']
    }
    verify_equal('false', TIMEOUT, 'Dash fullscreen key was not unset') {
        ($SUT.execute_shell_command "dconf read #{DASH_FULLSCREEN_KEY}").chop
    }
  end

  test "Dash fullscreen initially" do
    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} true"
    $SUT.execute_shell_command "dconf write #{DASH_FORMFACTOR_KEY} 'desktop'"
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @shell.ShellDeclarativeView()['dashActive']
    }
    verify_equal('FullScreenMode', TIMEOUT, 'Dash initial state is wrong') {
        @shell.ShellDeclarativeView()['dashMode']
    }
  end

  test "Dash always fullscreen if not desktop form factor" do
    $SUT.execute_shell_command "dconf write #{DASH_FULLSCREEN_KEY} false"
    $SUT.execute_shell_command "dconf write #{DASH_FORMFACTOR_KEY} 'tv'"
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @shell.ShellDeclarativeView()['dashActive']
    }
    verify_equal('FullScreenMode', TIMEOUT, 'Dash initial state is wrong') {
        @shell.ShellDeclarativeView()['dashMode']
    }
  end
end
