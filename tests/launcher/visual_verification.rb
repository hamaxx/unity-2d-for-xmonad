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

############################# Test Suite #############################
context "Launcher Autohide and Show Tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'

  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-launcher'
    $SUT.execute_shell_command 'killall unity-2d-launcher'
  end
  
  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_LAUNCHER, 
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-launcher'
  end

  #####################################################################################
  # Test cases

  test "Visually compare Dash tile with reference" do
    expected_image = pwd + 'verification/dash-tile.png'
    
    dash_tile = @app.LauncherList( :name => 'main' ).LauncherList( :isBfb => true ).QDeclarativeItem()

    #Check tile visual matches reference image, with some tolerance.
    assert( dash_tile.find_on_screen(expected_image, 4), \
        'Dash tile not matching reference image' )
  end
  
  test "Check Dash Tile location in Launcher" do
    expected_image = pwd + 'verification/dash-tile.png'
    
    tile_list = @app.Unity2dPanel().LauncherList( :name => 'main' )
  
    # Given the reference image, locate the matching visual in the LauncherList
    coordinates = tile_list.find_on_screen(expected_image, 4)
    assert( coordinates, 'Unable to find visual matching Dash tile reference image on screen' )
    
    # Dash tile should have these coordinates in the Launcher
    assert_equal( coordinates, [7,7], 'Dash tile not at correct coordinates' )
  end

end
