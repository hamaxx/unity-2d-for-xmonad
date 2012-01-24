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

############################# Test Suite #############################
context "Dash Lense Filter Results renderer tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'

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
    # Execute the application
    @sut = TDriver.sut(:Id => "sut_qt")
    @app = @sut.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability",
                     :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    #@app.close
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-shell"
  end

  #####################################################################################
  # Test cases

  test "Verify renderer filter-check-option" do
    XDo::Keyboard.super
    verify (TIMEOUT) { @app.Dash() }

    button = ""
    verify( TIMEOUT, 'Could not find AppLensButton' ) {
        button = @app.Dash().LensBar().LensButton( :name => 'Applications' )
    }

    XDo::Mouse.move(button['x_absolute'].to_i + 1, button['y_absolute'].to_i + 1, 0, true)
    XDo::Mouse.click(nil, nil, :left)

    button = ""
    verify( TIMEOUT, 'Could not find filterResults button' ) {
        button = @app.Dash().FilterPane().AbstractButton( :name => 'filterResults' )
    }

    XDo::Mouse.move(button['x_absolute'].to_i + 1, button['y_absolute'].to_i + 1, 0, true)
    XDo::Mouse.click(nil, nil, :left)


    loader = ""
    verify( TIMEOUT, 'Could not find FilterCheckOption Loader' ) {
        loader = @app.Dash().FilterPane().FilterLoader(:name => 'filter-checkoption').QDeclarativeLoader( :name => 'filter-checkoption' )
    }

    verify( TIMEOUT, 'FilterCheckOption don\'t have two columns' ) {
        loader.GridViewWithSpacing()['columns'] == '2'
    }

  end

  test "Verify renderer filter-check-option-compact" do
    XDo::Keyboard.super
    verify (TIMEOUT) { @app.Dash() }

    button = ""
    verify( TIMEOUT, 'Could not find AppLensButton' ) {
        button = @app.Dash().LensBar().LensButton( :name => 'Music' )
    }

    XDo::Mouse.move(button['x_absolute'].to_i + 1, button['y_absolute'].to_i + 1, 0, true)
    XDo::Mouse.click(nil, nil, :left)

    button = ""
    verify( TIMEOUT, 'Could not find filterResults button' ) {
        button = @app.Dash().FilterPane().AbstractButton( :name => 'filterResults' )
    }

    XDo::Mouse.move(button['x_absolute'].to_i + 1, button['y_absolute'].to_i + 1, 0, true)
    XDo::Mouse.click(nil, nil, :left)


    loader = ""
    verify( TIMEOUT, 'Could not find FilterCheckOption Loader' ) {
        loader = @app.Dash().FilterPane().FilterLoader(:name => 'filter-checkoption-compact').QDeclarativeLoader( :name => 'filter-checkoption-compact' )
    }

    verify( TIMEOUT, 'FilterCheckOptionCompact don\'t have three columns' ) {
        loader.GridViewWithSpacing()['columns'] == '3'
    }

  end
end
