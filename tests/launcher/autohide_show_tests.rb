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

############################# Test Suite #############################
context "Launcher Autohide and Show Tests" do
  launcher_favorites = ""

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
    		         :arguments => "-testability", 
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


  # Test case objectives:
  #   * Check the Launcher position on Empty desktop
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Verify Launcher is #{LAUNCHER_WIDTH} pixels wide
  #   * Verify Launcher showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Position with Empty Desktop" do
    # check width before proceeding
    verify_equal( LAUNCHER_WIDTH, TIMEOUT, "Launcher is not #{LAUNCHER_WIDTH} pixels wide on screen!" ) {
      @app.Unity2dPanel()['width'].to_i
    }

    verify_equal( 0, TIMEOUT, 'Launcher hiding on empty desktop, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
  end


  # Test case objectives:
  #   * Check the Launcher position on desktop with window not in way
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position not overlapping Launcher
  #   * Verify Launcher showing
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Position with Window not in the way" do
    # Open Terminal with position 100x100
    xid = TmpWindow.open_window_at(100,100)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Check the Launcher position on desktop with window in the way
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hides
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Position with Window in the way" do
    # Open Terminal with position 40x100
    xid = TmpWindow.open_window_at(40,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible when window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Check Launcher autohide working
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position not overlapping Launcher
  #   * Verify Launcher showing
  #   * Move application window to position overlapping Launcher
  #   * Verify Launcher hides
  #   * Move application window to position not overlapping Launcher
  #   * Verify Launcher shows again
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Move window positioning to check launcher action" do
    # Open Terminal with position 100x100
    xid = TmpWindow.open_window_at(100,100)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    xid.move(LAUNCHER_WIDTH-1,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible when window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    xid.move(LAUNCHER_WIDTH,100)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Check Launcher reveal using mouse works
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Move mouse to left of screen to reveal Launcher
  #   * Verify Launcher shows but not immediately
  #   * Move mouse to the right, but still over the Launcher
  #   * Verify Launcher still showing
  #   * Move mouse further right to not overlap Launcher
  #   * Verify Launcher hides but not immediately
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Reveal hidden Launcher with mouse" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::Mouse.move(0,200)
    sleep 0.4
    verify_equal( -LAUNCHER_WIDTH, 0, 'Launcher should not be visible immediately after mouse moves to the left edge, has to wait 0.5 seconds to show' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    verify_equal( 0, TIMEOUT, 'Launcher hiding when mouse at left edge of screen' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::Mouse.move(LAUNCHER_WIDTH-1,200)
    verify_equal( 0, TIMEOUT, 'Launcher should still be visible as mouse over it' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::Mouse.move(LAUNCHER_WIDTH,200)
    sleep 0.9
    verify_equal( 0, 0, 'Launcher should still be visible as it should take 1 second to hide after mouse is not over it' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Check Launcher reveal with Super key works
  # Pre-conditions
  #   * None
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Hold down Super key
  #   * Verify Launcher shows
  #   * Verify Tile shortcut numbers are showing 
  #   * Release Super key
  #   * Verify Launcher hides
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Hold Super key down to reveal launcher and shortcut keys" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    sleep 1 #launcher seems not ready to accept Super key, need a pause
    XDo::Keyboard.key_down('SUPER')
    verify_equal( 0, TIMEOUT, 'Launcher hiding when Super Key held, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    verify_equal( 'true', TIMEOUT, 'Shortcut on Home Folder icon not displaying with Super key held' ) {
      @app.LauncherList( :name => 'main' ) \
          .QDeclarativeItem( :name => 'Home Folder' ) \
          .QDeclarativeRectangle() \
          .QDeclarativeText()['visible']
    }  

    XDo::Keyboard.key_up('SUPER')
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Alt+F1 gives keyboard focus to Launcher, escape removes it
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Press Alt+F1
  #   * Verify Launcher shows
  #   * Verify Dash icon is highlighted
  #   * Press Escape
  #   * Verify Launcher hides
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Press Alt+F1 to focus Launcher" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_equal( 0, TIMEOUT, 'Launcher hiding after Alt+F1 pressed, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    verify_equal( 'true', TIMEOUT, 'Dash icon not highlighted after Alt+F1 pressed' ){
      @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true ).QDeclarativeImage( :name => 'selectionOutline' )['visible']
    }

    XDo::Keyboard.escape
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end


  # Test case objectives:
  #   * Alt+F1 takes & gives keyboard focus to the Launcher
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Verify application has keyboard focus
  #   * Press Alt+F1
  #   * Verify Launcher shows
  #   * Verify Dash icon is highlighted
  #   * Verify application does not have keyboard focus
  #   * Press Alt+F1
  #   * Verify Launcher hides
  #   * Verify application has keyboard focus
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Press Alt+F1 to focus/unfocus Launcher" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    assert_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal should have focus after starting it' )

    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_equal( 0, TIMEOUT, 'Launcher hiding after Alt+F1 pressed, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    verify_equal( 'true', TIMEOUT, 'Dash icon not highlighted after Alt+F1 pressed' ) {
      @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true ).QDeclarativeImage( :name => 'selectionOutline' )['visible']
    }
    assert_not_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal has focus when it should be in the launcher' )

    XDo::Keyboard.alt_F1
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way and mouse moved out, should be hidden' ){
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    assert_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal does not have focus when it should' )
    xid.close!
  end


  # Test case objectives:
  #   * Launcher displays when 'show desktop' engages
  # Pre-conditions
  #   * Desktop with no running applications
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Engage "Show Desktop" mode
  #   * Verify Launcher showing
  #   * Disengage "Show Desktop" mode
  #   * Verify Launcher hides
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher visible on show-desktop" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::XWindow.toggle_minimize_all # This is effectively the show-desktop shortcut
    verify_equal( 0, TIMEOUT, 'Launcher hiding after triggering show-desktop, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::XWindow.toggle_minimize_all
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  # Test case objectives:
  #   * Launcher is around for 1 second after removing a tile
  # Pre-conditions
  #   * Desktop with at least one application not running
  # Test steps
  #   * Open application in position overlapping Launcher
  #   * Verify Launcher hiding
  #   * Right click on a non running application tile
  #   * Wait enough time ( > 1 sec) so that the timer that would close the launcher
  #     if the menu was not open triggers
  #   * Click on the last menu item (Remove)
  #   * Verify Launcher stays away for a second
  # Post-conditions
  #   * None
  # References
  #   * None
  test "Launcher hide delay on tile removal" do
    xid = TmpWindow.open_window_at(10,100)
    tiles = ""
    verify( 0, 'Could not find any non running application tile to remove' ) {
        tiles = @app.LauncherList( :name => 'main' ).children( { :running => 'false', :desktopFile => /^.*.desktop$/ } )
    }
    if !tiles.empty?
      tile = tiles[0]
      XDo::Mouse.move(0, 200, 0, true)
      verify_equal( 0, TIMEOUT, 'Launcher hiding when mouse at left edge of screen' ) {
        @app.Unity2dPanel()['x_absolute'].to_i
      }
      XDo::Mouse.move(tile['x_absolute'].to_i + 1, tile['y_absolute'].to_i + 1, 0, true)
      XDo::Mouse.click(nil, nil, :right)
      menu = @app.LauncherContextualMenu( :folded => false );
      XDo::Mouse.move(menu['x_absolute'].to_i + 20, menu['y_absolute'].to_i + menu['height'].to_i - 10, 0, true)
      sleep 1.5
      XDo::Mouse.click()
      sleep 0.9
      verify_equal( 0, 0, 'Launcher hiding after icon removal, should be visible for 1 second' ) {
        @app.Unity2dPanel()['x_absolute'].to_i
      }
      verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
        @app.Unity2dPanel()['x_absolute'].to_i
      }
      xid.close!
    end
  end

  # Test case objectives:
  # * Launcher does not hide after toggling the dash
  # Pre-conditions
  # * Desktop with no running applications
  # Test steps
  # * Open application in position overlapping Launcher
  # * Verify Launcher hiding
  # * Move mouse to the left
  # * Verify Launcher showing
  # * Click twice in the bfb
  # * Verify Launcher showing during 1.5 seconds
  # Post-conditions
  # * None
  # References
  # * None
  test "Launcher visible after toggling dash" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
 
    bfb = @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true );
    XDo::Mouse.move(0, 200, 0, true)
    verify_equal( 0, TIMEOUT, 'Launcher hiding when mouse at left edge of screen' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    bfb.move_mouse()
    bfb.tap()
    bfb.tap()
    verify_not(0, 'Launcher hiding after hovering mouse over bfb and clicking twice') {
      verify_equal( -LAUNCHER_WIDTH, 2 ) {
        @app.Unity2dPanel()['x_absolute'].to_i
      }
    }
 
    xid.close!
  end

  # Test case objectives:
  # * Launcher does not hide on Esc after Alt+F1 with overlapping window
  # Pre-conditions
  # * Desktop with no running applications
  # Test steps
  # * Open application in position overlapping Launcher
  # * Verify Launcher hiding
  # * Press Alt+F1
  # * Verify Launcher showing
  # * Move mouse over the launcher
  # * Press Esc
  # * Verify Launcher does not hide
  # Post-conditions
  # * None
  # References
  # * None
  test "Launcher does not hide on Esc after Alt+F1 with overlapping window" do
    xid = TmpWindow.open_window_at(10,100)
    verify_equal( -LAUNCHER_WIDTH, TIMEOUT, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    bfb = @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true );
    bfb.move_mouse()
    XDo::Keyboard.escape
    verify_not(0, 'Launcher hiding after hovering mouse over bfb and clicking twice') {
      verify_equal( -LAUNCHER_WIDTH, 2 ) {
        @app.Unity2dPanel()['x_absolute'].to_i
      }
    }
    xid.close!
  end

end
