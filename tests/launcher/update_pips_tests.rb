#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Lohith DS <lohith.shivamurthy@canonical.com>
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

def open_window()
    window_id = -1
    o =  [('a'..'z'),('A'..'Z')].map{|i| i.to_a}.flatten;
    title_string  =  (0..50).map{ o[rand(o.length)]  }.join;
    if (@@title_string.empty?)
        @@title_string = title_string
    end
    # Open xman with random title
    system "xman -geometry 300x90+100+100 -title #{title_string} &"
    Timeout.timeout(30){ window_id = XDo::XWindow.wait_for_window(title_string)}
    Kernel.raise(SystemCallError, "Unable to open xman") if window_id == -1
    return XDo::XWindow.new(window_id)
end

def change_window_workspace(xid)
    new_workspace = 0
    if xid.desktop > 0
        new_workspace=0
    else
        new_workspace=1
    end

    xid.desktop=new_workspace
    verify_true(3, 'Change window workspace failed') {
        xid.desktop==new_workspace
    }
end

def reset_current_workspace
    XDo::XWindow.desktop=@@current_workspace
    verify_true(TIMEOUT, 'Reset active workspace failed') {
        XDo::XWindow.desktop==@@current_workspace
    }
end

############################# Test Suite #############################
context "Launcher pips tests" do
  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'killall unity-2d-shell > /dev/null 2>&1'
    system 'killall unity-2d-shell > /dev/null 2>&1'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all

    @number_of_workspaces = XDo::XWindow.desktop_num
    @reset_num_workspaces = false
    if @number_of_workspaces < 2
        puts ("Setting number of workspaces to minimum(two) to enable the test suite")
        XDo::XWindow.desktop_num = 2
        @reset_num_workspaces = true
    end

    system 'killall xman > /dev/null 2>&1'
    @@current_workspace = XDo::XWindow.desktop
    @@title_string = ""
    @@xid_list=Array.new

  end
  
  # Run once at the end of this test suite
  shutdown do
    if @reset_num_workspaces == true
        puts ("\nResetting number of workspaces to " + @number_of_workspaces.to_s )
        XDo::XWindow.desktop_num=@number_of_workspaces
    end
    @@xid_list.each {
        |xid|
        if (xid.exists?)
            xid.close!
        end
    }
  end

  # Run before each test case begins
  setup do
    #Ensure mouse out of the way

    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")    
    @app = @sut.run( :name => UNITY_2D_SHELL,
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    reset_current_workspace
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-shell"
  end

    #####################################################################################
    # Test cases

    # Test case objectives:
    #   * Check pips are updated properly when the app is in current workspace
    #   *
    # Pre-conditions
    #   * This test case assumes that the test app will have only one window
    #   * For eg, xman is choosen
    #   * To ensure there is calculator windows running, kill all the calculator windows if any
    #   *
    # Test steps
    #   * Open a calculator window
    #   * Verify the launcher tile pip image matches with the expected one
    #   *
    # Post-conditions
    #   *   Reset the current workspace to initial workspace
    #   *
    # References
    #   * lp:#883172
    #   *

    test "Update launcher pips to indicate a window in current workspace" do
        xid = open_window()
        @@xid_list << xid

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching with launcher_arrow_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end

    # Test case objectives:
    #   * Check pips are updated properly when the app is moved to different workspace
    #   *
    # Pre-conditions
    #   * The same window created in above test-case is re-used
    #   *
    # Test steps
    #   * Move the window to a differnt workspace
    #   * Verify the launcher tile pip image matches with the expected one
    #   *
    # Post-conditions
    #   * Reset the current workspace to initial workspace
    #   *
    # References
    #   * lp:#883172
    #   *
    test "Update launcher pips to indicate an app that belongs to different workspace" do
        xid = @@xid_list[0]

        change_window_workspace(xid)

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching with launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end

    # Test case objectives:
    #   * Check pips are updated properly with app with more than one window
    #   *
    # Pre-conditions
    #   * The same window created in first test case is re-used
    #   *
    # Test steps
    #   * Open another window alogn with first window from the testcase one.
    #   * Verify the launcher tile pip images matches with the expected one
    #   *
    # Post-conditions
    #   * Reset the current workspace to initial workspace
    #   *
    # References
    #   * lp:#883172
    #   *

    test "Update launcher pips to indicate an app with two windows on two different workspaces" do
        xid = open_window()
        @@xid_list << xid

        expected_pip_image = 'image://blended/launcher/artwork/launcher_pip_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'Pip not matching expected image launcher_pip_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end

    # Test case objectives:
    #   * Check pips are updated properly when the app is moved to another workspace
    #   *
    # Pre-conditions
    #   * The same windows created above are re-used
    #   *
    # Test steps
    #   * Move the second window too to a differnt workspace
    #   * Verify the launcher tile pip image matches with the expected one
    #   *
    # Post-conditions
    #   * Reset the current workspace to initial workspace
    #   *
    # References
    #   * lp:#883172
    #   *
    test "Update launcher pips to indicate an app completely belonging to different workspace." do
        xid1= @@xid_list[1]

        change_window_workspace(xid1)

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching expected image launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end

    # Test case objectives:
    #   * Check pips are updated properly when the app is moved to another workspace
    #   *
    # Pre-conditions
    #   * The same windows created above are resued.
    #   *
    # Test steps
    #   * Close one of the app window and move the last window back to its original workspace
    #   * Verify the launcher tile pip image matches with the expected one
    #   *
    # Post-conditions
    #   * Reset the current workspace to initial workspace
    #   *
    # References
    #   * lp:#883172
    #   *
    test "Update launcher pips to indicate when a app window is closed." do
        xid0 = @@xid_list[0]
        xid1 = @@xid_list[1]

        xid1.close!
        change_window_workspace(xid0)

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching expected image launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end
end
