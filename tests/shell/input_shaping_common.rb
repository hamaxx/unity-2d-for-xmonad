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

# We just need a temp file name but ruby insist on creating it, so we just delete it first.
def tempfilename(base, extension = ".tmp")
    maskfile = Tempfile.new([base, extension])
    maskpath = maskfile.path
    maskfile.close
    maskfile.unlink
    return maskpath
end

def get_shell_shape
    # Try to find the shell window
    shell_ids = XDo::XWindow::search('unity-2d-shell')
    assert(!shell_ids.empty?, "Failed to retrieve the shell window id")
    shell_id = shell_ids[0]

    # Get the shape of the shell window using our custom tool
    maskpath = tempfilename('shape', '.png')
    get_shape_command = BINARY_DIR + "/tests/getshape/getshape #{shell_id} #{maskpath}"
    $SUT.execute_shell_command get_shape_command
    $SUT.copy_from_sut(:from => '/tmp/', :file => maskpath, :to => '/tmp/')
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

def test_shape_of_launcher_alone(isRTL = false)
    XDo::Keyboard.alt_F1
    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be
    # then use imagemagick to create an image that contains only a black rectangle where the
    # launcher should be.
    screen_width, screen_height = XDo::XWindow.display_geometry()
    screen_height -= PANEL_HEIGHT
    comparepath = tempfilename('shape', '.png')
    flopstring = ""
    if isRTL
        flopstring = "-flop"
    end
    %x{convert -size #{screen_width}x#{screen_height} xc:white -background white -draw 'rectangle 0,0 #{LAUNCHER_WIDTH-1},#{screen_height}' #{flopstring} #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(0, "The actual shape does not match the expected shape") { identical }
end

def test_shape_of_launcher_and_desktop_mode_dash(isRTL = false)
    XDo::Keyboard.simulate('{SUPER}')
    sleep 1
    maskpath = get_shell_shape()

    # Since the shape of the launcher is dependent on screen geometry, calculate what it should be,
    # then draw a black rectangle and compose it at the left side of the dash verification image.

    screen_width, screen_height = XDo::XWindow.display_geometry()
    screen_height -= PANEL_HEIGHT

    pwd = File.expand_path(File.dirname(__FILE__))
    verifypath = pwd + "/verification/dash_desktop.png"

    comparepath = tempfilename('shape', '.png')

    flopstring = ""
    if isRTL
        flopstring = "-flop"
    end
    %x{convert xc:white -extent #{screen_width}x#{screen_height} #{verifypath} -geometry +#{LAUNCHER_WIDTH}+0 -composite  \
       \\( xc:black -background black -extent #{LAUNCHER_WIDTH}x#{screen_height} \\) \
       -gravity northwest -compose over -composite #{flopstring} #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(0, "The actual shape does not match the expected shape") { identical }
end

def test_shape_of_launcher_and_fullscreen_mode_dash(isRTL = false)
    XDo::Keyboard.simulate('{SUPER}')
    sleep 1
    @app.ShellDeclarativeView()['dashMode'] = 'FullScreenMode'
    sleep 1

    maskpath = get_shell_shape()

    # Compare with just one big rectangle filling the entire screen minus the panel area
    screen_width, screen_height = XDo::XWindow.display_geometry()
    screen_height -= PANEL_HEIGHT
    comparepath = tempfilename('shape', '.png')
    flopstring = ""
    if isRTL
        flopstring = "-flop"
    end
    %x{convert xc:black -background black -extent #{screen_width}x#{screen_height} #{flopstring} #{comparepath}}

    identical = compare_images(maskpath, comparepath)

    File.unlink(maskpath)
    File.unlink(comparepath)

    verify_true(0, "The actual shape does not match the expected shape") { identical }
end
