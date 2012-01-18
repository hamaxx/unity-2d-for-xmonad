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
require 'tempfile'
include TDriverVerify

# Helper function to open window at certain position
def open_window_at(x, y, w = 100, h = 30)
  # Open Terminal with position (x,y)
  Dir.mktmpdir {|dir| # use this to generate unique window title to help Xdo get window ID
    system "gnome-terminal --geometry=#{w}x#{h}+#{x}+#{y} --working-directory=#{dir} &"
    Timeout.timeout(3){XDo::XWindow.wait_for_window(dir)}
  }
  return XDo::XWindow.from_active
end

def desktop_geometry
    out = %x{xdotool getdisplaygeometry}
    return out.split.collect { |coord| coord.to_i }
end

# We just need a temp file name but ruby insist on creating it, so we just delete it first.
def tempfilename(base, extension = ".tmp")
    maskfile = Tempfile.new([base, extension])
    maskpath = maskfile.path
    maskfile.close
    maskfile.unlink
    return maskpath
end

def get_shell_shape
    pwd = File.expand_path(File.dirname(__FILE__))

    # Try to find the shell window
    shell_ids = XDo::XWindow::search('unity-2d-shell')
    assert( !shell_ids.empty?, "Failed to retrieve the shell window id");
    shell_id = shell_ids[0]

    # Get the shape of the shell window using our custom tool
    maskpath = tempfilename('shape', '.png')
    out = %x{#{pwd}/../getshape/getshape #{shell_id} #{maskpath}}
    assert( $?.exitstatus == 0, "Failed to call getshape to get the shape of the window")

    return maskpath
end

############################# Test Suite #############################
context "Launcher Autohide and Show Tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'

  PANEL_HEIGHT = 24
  LAUNCHER_WIDTH = 65
  DASH_HEIGHT_COLLAPSED = 115
  DASH_HEIGHT = 615
  DASH_WIDTH = 996

  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-shell > /dev/null 2>&1'
    system 'killall unity-2d-shell > /dev/null 2>&1'

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
    @app = @sut.run( :name => UNITY_2D_SHELL, 
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
    # Make certain application is ready for testing
    verify(10){ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-shell"
  end

  #####################################################################################
  # Test cases

  test "Shape of launcher alone" do
    XDo::Keyboard.alt_F1
    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be
    # then use imagemagick to create an image that contains only a black rectangle where the
    # launcher should be.
    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT
    comparepath = tempfilename('shape', '.png')
    %x{convert xc:black -background black -extent #{LAUNCHER_WIDTH}x#{screen_height} #{comparepath}}

    # Compare the two files with imagemagick and return discard the difference image, just
    # use numeric comparison result. Metric "AE" returns the amount of different pixels.
    # Also redirect stderr to stdout since the result of the comparison will be output to stderr.
    out = %x{compare #{maskpath} #{comparepath} -metric AE /dev/null 2>&1}

    File.unlink(maskpath)
    File.unlink(comparepath)

    assert( out.chop == "0", "The shape is wrong")
  end

  test "Shape of launcher and desktop mode dash" do
    XDo::Keyboard.simulate('{SUPER}')
    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be,
    # then paint it as a black rectangle at the left side of the dash verification image.

    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT

    out = %x{identify -format "%wx%h" #{pwd}/verification/dash_desktop.png}
    verify_width = out.split("x")[0].to_i
    verify_width += LAUNCHER_WIDTH

    comparepath = tempfilename('shape', '.png')

    %x{convert #{pwd}/verification/dash_desktop.png \
       -gravity northeast -extent #{verify_width}x#{screen_height}! \
       \\( xc:black -background black -extent #{LAUNCHER_WIDTH}x#{screen_height} \\) \
       -gravity northwest -compose over -composite #{comparepath}}

    # Compare the two files with imagemagick and return discard the difference image, just
    # use numeric comparison result. Metric "AE" returns the amount of different pixels.
    # Also redirect stderr to stdout since the result of the comparison will be output to stderr.
    difference = %x{compare #{maskpath} #{comparepath} -metric AE /dev/null 2>&1}.chop.to_i

    File.unlink(maskpath)
    File.unlink(comparepath)

    assert_equal(0, difference, "The shape is wrong")
  end

  test "Shape of launcher and fullscreen mode dash" do
    XDo::Keyboard.simulate('{SUPER}')

    # I could've clicked in the bottom-right corner, but this seems more future-proof
    @app.ShellDeclarativeView()['dashMode'] = 'FullScreenMode'

    maskpath = get_shell_shape()

    # Compare with just one big rectangle filling the entire screen minus the panel area
    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT
    comparepath = tempfilename('shape', '.png')
    %x{convert xc:black -background black -extent #{screen_width}x#{screen_height} #{comparepath}}

    difference = %x{compare #{maskpath} #{comparepath} -metric AE /dev/null 2>&1}.chop.to_i

    File.unlink(maskpath)
    File.unlink(comparepath)

    assert_equal(0, difference, "The shape is wrong")
  end
end
