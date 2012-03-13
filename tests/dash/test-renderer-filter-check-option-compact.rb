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
context "Dash - Filter Results renderer tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'
  end

  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    # Execute the application
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability",
                     :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    $SUT.execute_shell_command "pkill -nf unity-2d-shell"
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Verify renderer filter-check-option is displayed with two columns
  # Pre-conditions
  #   * None
  # Test steps
  #   * Open Dash
  #   * Goto Applications Lens
  #   * Open Filter Results
  #   * verify that filter-checkoption renderer is having two columns
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify renderer filter-check-option" do
    XDo::Keyboard.super
    verify (TIMEOUT) { @app.Dash() }

    button = ""
    verify( TIMEOUT, 'Could not find Applications Lens. Did you install it?' ) {
        button = @app.Dash().LensBar().LensButton( :name => 'Applications' )
    }

    button.tap

    verify( TIMEOUT, 'Could not find filterResults button' ) {
        button = @app.Dash().FilterPane().AbstractButton( :name => 'filterResults' )
    }

    button.tap

    loader = ""
    verify( TIMEOUT, 'Could not find FilterCheckOption Loader' ) {
        loader = @app.Dash().FilterPane() \
                     .children( :type => 'FilterLoader', :name => 'filter-checkoption')[0] \
                     .QDeclarativeLoader()
    }

    verify_equal(2, TIMEOUT, 'FilterCheckOption don\'t have two columns' ) {
        loader.GridViewWithSpacing()['columns'].to_i
    }

  end

  # Test case objectives:
  #   * Verify renderer filter-check-option-compact is displayed with three columns
  # Pre-conditions
  #   * None
  # Test steps
  #   * Open Dash
  #   * Goto Music Lens
  #   * Open Filter-Results
  #   * verify that filter-checkoption-compact renderer is having three columns
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify renderer filter-check-option-compact" do
    XDo::Keyboard.super
    verify (TIMEOUT) { @app.Dash() }

    button = ""
    verify( TIMEOUT, 'Could not find Music Lens. Did you install it?' ) {
        button = @app.Dash().LensBar().LensButton( :name => 'Music' )
    }

    button.tap

    verify( TIMEOUT, 'Could not find filterResults button' ) {
        button = @app.Dash().FilterPane().AbstractButton( :name => 'filterResults' )
    }

    button.tap

    loader = ""
    verify( TIMEOUT, 'Could not find FilterCheckOptionCompact Loader' ) {
        loader = @app.Dash().FilterPane().FilterLoader(:name => 'filter-checkoption-compact').QDeclarativeLoader( :name => 'filter-checkoption-compact' )
    }

    verify_equal(3, TIMEOUT, 'FilterCheckOptionCompact don\'t have three columns' ) {
        loader.GridViewWithSpacing()['columns'].to_i
    }
    end
end
