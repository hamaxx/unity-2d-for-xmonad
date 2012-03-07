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

############################# Test Suite #############################
context "Launcher Contextual Menu Tests" do
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
                     :arguments => "-testability",
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

  # Test case objectives:
  #   * Check that the hint is displayed on mouse hover
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Hover the cursor over the first application entry
  #   * Check that the hint is displayed
  #   * Check that there are two QActions (there is a "ghost" QAction there)
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Display launcher item hint" do
    tiles = ""
    verify( TIMEOUT, 'Could not find any application tile' ) {
      tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
    }
    tile = tiles[0]
    tile.move_mouse()
    verify(TIMEOUT, 'The launcher item hint is not visible' ) {
      @app.LauncherContextualMenu()
    }
    actions = ""
    verify( TIMEOUT, 'Could not find any actions in the menu' ) {
      actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
    }
    verify_equal( 2, TIMEOUT, 'There was an unexpected number of actions in the menu' ) {
      actions.count
    }
  end

  # Test case objectives:
  #   * Check that the menu is displayed on right click
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Right click the first application entry
  #   * Check that the menu is displayed
  #   * Check that there are at least four QActions (two actions and separator, plus a "ghost" one)
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Display launcher menu after right click" do
    tiles = ""
    verify( TIMEOUT, 'Could not find any application tile' ) {
      tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
    }
    tile = tiles[0]
    tile.move_mouse()
    tile.tap(1, :Right)
    verify(TIMEOUT, 'The launcher menu is not visible' ) {
      @app.LauncherContextualMenu()
    }
    actions = ""
    verify( TIMEOUT, 'Could not find any actions in the menu' ) {
      actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
    }
    verify_true( TIMEOUT, 'There was not enough actions in the menu' ) {
      actions.count >= 4
    }
  end

  # Test case objectives:
  #   * Check that the menu is displayed on right key from item
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go down and right
  #   * Check that the menu is displayed
  #   * Check that there are at least four QActions (two actions and separator, plus a "ghost" one)
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Display launcher menu with keyboard navigation" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.down
    XDo::Keyboard.right
    verify(TIMEOUT, 'The launcher menu is not visible' ) {
      @app.LauncherContextualMenu()
    }
    actions = ""
    verify( TIMEOUT, 'Could not find any actions in the menu' ) {
      actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
    }
    verify_true( TIMEOUT, 'There was not enough actions in the menu' ) {
      actions.count >= 4
    }
  end

  # Test case objectives:
  #   * Check that the menu is being closed on Esc
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go down and right, press Esc
  #   * Check that the menu is no longer there
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Close launcher menu when pressing Esc" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.down
    XDo::Keyboard.right
    XDo::Keyboard.escape
    verify_not(TIMEOUT, 'The launcher menu is not visible' ) {
      @app.LauncherContextualMenu()
    }
  end

  # Test case objectives:
  #   * Check that the menu is being closed on left key
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go down and right, then left
  #   * Check that the menu is no longer there
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Close launcher menu when navigating back to the launcher" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.down
    XDo::Keyboard.right
    XDo::Keyboard.left
    verify_not(TIMEOUT, 'The launcher menu is not visible' ) {
      @app.LauncherContextualMenu()
    }
  end

  # Test case objectives:
  #   * Check that the focus goes back to the launcher item when menu was dismissed with Esc
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go down and right, press Esc
  #   * Check that the launcher item has focus
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify launcher tile gets focus after dismissing the menu with Esc" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.down
    XDo::Keyboard.right
    XDo::Keyboard.escape
    tiles = ""
    verify( TIMEOUT, 'Could not find any application tile' ) {
      tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
    }
    tile = tiles[0]
    verify_equal( "true", TIMEOUT, 'Launcher item didn\'t regain focus' ) {
      tile['activeFocus']
    }
  end

  # Test case objectives:
  #   * Check that the focus goes back to the launcher item when menu was dismissed with keyboard navigation
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go down and right, press Esc
  #   * Check that the launcher item has focus
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify launcher tile gets focus after dismissing the menu with keyboard navigation" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.down
    XDo::Keyboard.right
    XDo::Keyboard.left
    tiles = ""
    verify( TIMEOUT, 'Could not find any application tile' ) {
      tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
    }
    tile = tiles[0]
    verify_equal( "true", TIMEOUT, 'Launcher item didn\'t regain focus' ) {
      tile['activeFocus']
    }
  end

  # Test case objectives:
  #   * Check that the focus goes from launcher menu to dash
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Focus the launcher, go right, press Super
  #   * Check that the dash search entry has focus
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Verify dash search entry gets focus after dismissing the menu with Super" do
    XDo::Keyboard.alt_F1
    XDo::Keyboard.right
    XDo::Keyboard.super
    verify_equal( "true", TIMEOUT, 'Dash search entry doesn\'t have focus' ) {
      @app.SearchEntry()['activeFocus']
    }
  end
end
