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
    assert(!shell_ids.empty?, "Failed to retrieve the shell window id")
    shell_id = shell_ids[0]

    # Get the shape of the shell window using our custom tool
    maskpath = tempfilename('shape', '.png')
    binary_dir = File.open('../misc/binary_dir.txt').first.strip
    out = %x{#{binary_dir}/tests/getshape/getshape #{shell_id} #{maskpath}}
    assert($?.exitstatus == 0, "Failed to call getshape to get the shape of the window")
    return maskpath
end

# Compare two images using ImageMagick. Perform a pixel-by-pixel comparison and return true
# only if all the pixels are identical.
def compare_images(image1, image2)
    # Before checking the pixels make sure the two images are the same size, otherwise some times
    # IM will hang for a long time (probably trying to do some super clever subimage matching).
    size1 = %x{identify -format '%wx%h' #{image1}}
    size2 = %x{identify -format '%wx%h' #{image2}}
    return false if size1 != size2

    # Discard the difference image and redirect stderr to stdout as IM will output the number of
    # different pixels there for some reason.
    difference = %x{compare #{image1} #{image2} -metric AE /dev/null 2>&1}.chop.to_i

    # If the images are too different or have different sizes IM will exit with return code 1.
    # Unfortunately it's the same code we have when there is any error (like a wrong filename),
    # so the best we can do is just return false
    return false if $?.exitstatus != 0

    return difference == 0
end

############################# Test Suite #############################
context "Shell input shape tests" do
  pwd = File.expand_path(File.dirname(__FILE__)) + '/'

  PANEL_HEIGHT = 24
  LAUNCHER_WIDTH = 65

  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-shell > /dev/null 2>&1'
    system 'killall unity-2d-shell > /dev/null 2>&1'
    system 'killall unity-2d-panel > /dev/null 2>&1'

    # Need panel running as position of shell depends on it
    @@sut = TDriver.sut(:Id => "sut_qt")
    @@panel = @@sut.run(:name => UNITY_2D_PANEL,
                        :arguments => "-testability" )

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
    @app = @sut.run(:name => UNITY_2D_SHELL,
                    :arguments => "-testability",
                    :sleeptime => 2)
    # Make certain application is ready for testing
    verify(10){ @app.Launcher() }
  end

  # Run after each test case completes
  teardown do
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

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(10, "The actual shape does not match the expected shape") { identical }
  end

  test "Shape of launcher and desktop mode dash" do
    XDo::Keyboard.simulate('{SUPER}')
    sleep 1
    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be,
    # then draw a black rectangle and compose it at the left side of the dash verification image.

    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT

    verifypath = "#{pwd}/verification/dash_desktop.png"
    out = %x{identify -format "%wx%h" #{verifypath}}
    verify_width = out.split("x")[0].to_i
    verify_width += LAUNCHER_WIDTH

    comparepath = tempfilename('shape', '.png')

    %x{convert #{verifypath} \
       -gravity northeast -extent #{verify_width}x#{screen_height}! \
       \\( xc:black -background black -extent #{LAUNCHER_WIDTH}x#{screen_height} \\) \
       -gravity northwest -compose over -composite #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(10, "The actual shape does not match the expected shape") { identical }
  end

  test "Shape of launcher and fullscreen mode dash" do
    XDo::Keyboard.simulate('{SUPER}')
    sleep 1
    @app.ShellDeclarativeView()['dashMode'] = 'FullScreenMode'
    sleep 1

    maskpath = get_shell_shape()

    # Compare with just one big rectangle filling the entire screen minus the panel area
    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT
    comparepath = tempfilename('shape', '.png')
    %x{convert xc:black -background black -extent #{screen_width}x#{screen_height} #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(10, "The actual shape does not match the expected shape") { identical }
  end

  test "Shape of launcher and collapsed desktop mode dash" do
    XDo::Keyboard.simulate('{SUPER}')
    sleep 1
    @app.AbstractButton(:name => 'closeShortcutsButton').tap
    sleep 1

    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be,
    # then draw a black rectangle and compose it at the left side of the dash verification image.

    screen_width, screen_height = desktop_geometry()
    screen_height -= PANEL_HEIGHT

    verifypath = "#{pwd}/verification/dash_collapsed.png"
    out = %x{identify -format "%wx%h" #{verifypath}}
    verify_width = out.split("x")[0].to_i
    verify_width += LAUNCHER_WIDTH

    comparepath = tempfilename('shape', '.png')

    %x{convert #{verifypath} \
       -gravity northeast -extent #{verify_width}x#{screen_height}! \
       \\( xc:black -background black -extent #{LAUNCHER_WIDTH}x#{screen_height} \\) \
       -gravity northwest -compose over -composite #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(10, "The actual shape does not match the expected shape") { identical }
  end
end
