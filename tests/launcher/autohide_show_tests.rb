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
require 'timeout'
require 'tmpdir'

# Helper function to open window at certain position
def open_window_at(x,y)
  # Open Terminal with position (x,y)
  Dir.mktmpdir {|dir| # use this to generate unique window title to help Xdo get window ID
    system "gnome-terminal --geometry=100x30+#{x}+#{y} --working-directory=#{dir} &"
    Timeout.timeout(3){XDo::XWindow.wait_for_window(dir)}
  }
  return XDo::XWindow.from_active
end

############################# Test Suite #############################
context "Launcher Autohide and Show Tests" do
  WIDTH = 65 #launcher bar width

  # Run once at the beginning of this test suite
  startup do
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
    #Ensure mouse out of the way
    XDo::Mouse.move(200,200,10,true)    

    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")    
    @app = @sut.run( :name => UNITY_2D_LAUNCHER, 
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.Unity2dPanel() }
  end

  # Run after each test case completes
  teardown do
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-launcher"
  end

  #####################################################################################
  # Test cases

  test "Position with Empty Desktop" do
    # check width before proceeding
    verify_equal( WIDTH, 30, "Launcher is not #{WIDTH} pixels wide on screen!" ) {
      @app.Unity2dPanel()['width'].to_i
    }

    verify_equal( 0, 30,'Launcher hiding on empty desktop, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
  end

  test "Position with Window not in the way" do
    # Open Terminal with position 100x100
    xid = open_window_at(100,100)
    verify_equal( 0, 30, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  test "Position with Window in the way" do
    # Open Terminal with position 40x100
    xid = open_window_at(40,100)
    verify_equal( -WIDTH, 30, 'Launcher visible when window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  test "Move window positioning to check launcher action" do
    # Open Terminal with position 100x100
    xid = open_window_at(100,100)
    verify_equal( 0, 30, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.move(WIDTH-1,100)
    verify_equal( -WIDTH, 30, 'Launcher visible when window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.move(WIDTH,100)
    verify_equal( 0, 30, 'Launcher hiding when window not in the way, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end
  
  test "Reveal hidden Launcher with mouse" do
    xid = open_window_at(10,100)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Mouse.move(0,200)
    verify_equal( 0, 30, 'Launcher hiding when mouse at left edge of screen' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Mouse.move(WIDTH-1,200)
    verify_equal( 0, 30, 'Launcher should still be visible as mouse over it' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Mouse.move(WIDTH,200)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  test "Hold Super key down to reveal launcher and shortcut keys" do
    xid = open_window_at(10,100)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::Keyboard.key_down('SUPER')
    verify_equal( 0, 30, 'Launcher hiding when Super Key held, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    
    verify_equal( 'true', 30, 'Shortcut on Home Folder icon not displaying with Super key held' ) {
      @app.LauncherList( :name => 'main' ) \
          .QDeclarativeItem( :name => 'Home Folder' ) \
          .QDeclarativeRectangle() \
          .QDeclarativeText()['visible']
    }
                  
    XDo::Keyboard.key_up('SUPER')
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  test "Press Alt+F1 to focus Launcher" do
    xid = open_window_at(10,100)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    
    verify_equal( 0, 30, 'Launcher hiding after Alt+F1 pressed, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    verify_equal( 'true', 30, 'Dash icon not highlighted after Alt+F1 pressed' ){
      @app.LauncherList( :name => 'main' ) \
          .QDeclarativeItem( :name => 'Dash home' ) \
          .QDeclarativeImage( :name => 'selectionOutline' )['visible']
    }
    XDo::Keyboard.escape

    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way and mouse moved out, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

  test "Press Alt+F1 to focus/unfocus Launcher" do
    xid = open_window_at(10,100)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    assert_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal should have focus after starting it' )
    XDo::Keyboard.alt_F1 #Must use uppercase F to indicate function keys
    verify_equal( 0, 30, 'Launcher hiding after Alt+F1 pressed, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    verify_equal( 'true', 30, 'Dash icon not highlighted after Alt+F1 pressed' ) {
      @app.LauncherList( :name => 'main' ) \
          .QDeclarativeItem( :name => 'Dash home' ) \
          .QDeclarativeImage( :name => 'selectionOutline' )['visible']
    }
    assert_not_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal has focus when it should be in the launcher' )
    XDo::Keyboard.alt_F1
    
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way and mouse moved out, should be hidden' ){
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    assert_equal( xid.id, XDo::XWindow.active_window, \
                  'terminal does not have focus when it should' )
    xid.close!
  end

  test "Launcher visible on show-desktop" do
    xid = open_window_at(10,100)
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    XDo::XWindow.toggle_minimize_all # This is effectively the show-desktop shortcut
    verify_equal( 0, 30, 'Launcher hiding after triggering show-desktop, should be visible' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }

    XDo::XWindow.toggle_minimize_all
    verify_equal( -WIDTH, 30, 'Launcher visible with window in the way, should be hidden' ) {
      @app.Unity2dPanel()['x_absolute'].to_i
    }
    xid.close!
  end

end
