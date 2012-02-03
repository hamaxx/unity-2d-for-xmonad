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
    system 'killall unity-2d-places > /dev/null 2>&1'
    system 'killall unity-2d-panel > /dev/null 2>&1'
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'killall unity-2d-launcher > /dev/null 2>&1'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end
  
  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    @fullscreen_old = %x{dconf read #{DASH_FULLSCREEN_KEY}}.chop
    @formfactor_old = %x{dconf read #{DASH_FORMFACTOR_KEY}}.chop

    # Execute the application
    @sut = TDriver.sut(:Id => "sut_qt")
    @dash = @sut.run(:name => UNITY_2D_PLACES,
                    :arguments => "-testability",
                    :sleeptime => 2)
    @panel = @sut.run(:name => UNITY_2D_PANEL,
                    :arguments => "-testability",
                    :sleeptime => 2)
    @launcher = @sut.run(:name => UNITY_2D_LAUNCHER,
                         :arguments => "-testability",
                         :sleeptime => 2)

    verify(10){ @panel.Unity2dPanel() }
    verify(10){ @launcher.Unity2dPanel() }
    # Dash can't be verified here as the declarative view is hidden until we make it active
  end

  # Run after each test case completes
  teardown do
    %x{dconf write #{DASH_FULLSCREEN_KEY} #{'"'}#{@fullscreen_old}#{'"'}}
    %x{dconf write #{DASH_FORMFACTOR_KEY} #{'"'}#{@formfactor_old}#{'"'}}

    system "pkill -nf unity-2d-panel"
    system "pkill -nf unity-2d-places"
    system "pkill -nf unity-2d-launcher"
  end

  def dash_always_fullscreen
    out = %x{xdotool getdisplaygeometry}
    width, height = out.split.collect { |coord| coord.to_i }
    return width < DASH_MIN_SCREEN_WIDTH && height < DASH_MIN_SCREEN_HEIGHT
  end

  #####################################################################################
  # Test cases

  test "Dash fullscreens on dconf key change" do
    %x{dconf write #{DASH_FULLSCREEN_KEY} false}
    %x{dconf write #{DASH_FORMFACTOR_KEY} #{'\"desktop\"'}}
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @dash.DashDeclarativeView()['active']
    }

    expected = dash_always_fullscreen ? 'FullScreenMode' : 'DesktopMode'
    verify_equal(expected, TIMEOUT, 'Dash is in the wrong fullscreen state') {
        @dash.DashDeclarativeView()['dashMode']
    }

    %x{dconf write #{DASH_FULLSCREEN_KEY} true}
    sleep 1

    verify_equal('FullScreenMode', TIMEOUT, 'Dash is not fullscreen but should be') {
        @dash.DashDeclarativeView()['dashMode']
    }
  end

  test "Dash reacts correctly to panel buttons" do
    %x{dconf write #{DASH_FULLSCREEN_KEY} false}
    %x{dconf write #{DASH_FORMFACTOR_KEY} #{'\"desktop\"'}}
    XDo::Keyboard.super
    sleep 1
    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @dash.DashDeclarativeView()['active']
    }

    maxbutton = nil
    verify(TIMEOUT, 'The "maximize" button did not appear when the dash was visible' ) {
        maxbutton = @panel.AppNameApplet().children( :type => 'QAbstractButton' )[2]
    }

    maxbutton.tap if maxbutton
    sleep 1
    verify_equal('FullScreenMode', TIMEOUT, 'Dash should be fullsceen, but it is not' ) {
        @dash.DashDeclarativeView()['dashMode']
    }

    # When always fullscreen tapping the max button does nothing, so the key should remain set to
    # false.
    expected = dash_always_fullscreen ? 'false' : 'true'
    verify_equal(expected, TIMEOUT, 'Dash fullscreen key has the wrong value after maximize') {
        %x{dconf read #{DASH_FULLSCREEN_KEY}}.chop
    }

    maxbutton.tap if maxbutton
    sleep 1
    expected = dash_always_fullscreen ? 'FullScreenMode' : 'DesktopMode'
    verify_equal(expected, TIMEOUT, 'Dash is in the wrong fullscreen state' ) {
        @dash.DashDeclarativeView()['dashMode']
    }
    verify_equal('false', TIMEOUT, 'Dash fullscreen key was not unset') {
        %x{dconf read #{DASH_FULLSCREEN_KEY}}.chop
    }
  end

  test "Dash fullscreen initially" do
    %x{dconf write #{DASH_FULLSCREEN_KEY} true}
    %x{dconf write #{DASH_FORMFACTOR_KEY} #{'\"desktop\"'}}
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @dash.DashDeclarativeView()['active']
    }
    verify_equal('FullScreenMode', TIMEOUT, 'Dash initial state is wrong') {
        @dash.DashDeclarativeView()['dashMode']
    }
  end

  test "Dash always fullscreen if not desktop form factor" do
    %x{dconf write #{DASH_FULLSCREEN_KEY} false}
    %x{dconf write #{DASH_FORMFACTOR_KEY} #{'\"tv\"'}}
    XDo::Keyboard.super
    sleep 1

    verify_equal('true', TIMEOUT, 'Dash did not appear') {
        @dash.DashDeclarativeView()['active']
    }
    verify_equal('FullScreenMode', TIMEOUT, 'Dash initial state is wrong') {
        @dash.DashDeclarativeView()['dashMode']
    }
  end

end
