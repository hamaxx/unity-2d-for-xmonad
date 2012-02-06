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
    # FIXME : Need to choose a better test application in future?
    window_id = -1
    o =  [('a'..'z'),('A'..'Z')].map{|i| i.to_a}.flatten;
    title_string  =  (0..50).map{ o[rand(o.length)]  }.join;
    if (@@title_string.empty?)
        @@title_string = title_string
    end
    # Open xman with random title
    $SUT.execute_shell_command("xman -geometry 300x90+100+100 -title #{title_string}", :detached => true)
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

def change_user_workspace
    new_workspace = 0
    if @@current_workspace > 0
        new_workspace = 0
    else
        new_workspace = 1
    end

    XDo::XWindow.desktop=new_workspace
    verify_true(TIMEOUT, 'Change user workspace failed') {
        XDo::XWindow.desktop==new_workspace
    }
end

def close_all_test_windows
    @@xid_list.each {
        |xid|
        if (xid.exists?)
            xid.close!
        end
    }
end

############################# Test Suite #############################
context "Launcher pips tests" do
  # Run once at the beginning of this test suite
  startup do
    $SUT.execute_shell_command 'killall unity-2d-shell'
    $SUT.execute_shell_command 'killall unity-2d-shell'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all

    @number_of_workspaces = XDo::XWindow.desktop_num
    @reset_num_workspaces = false
    if @number_of_workspaces < 2
        # Setting number of workspaces to minimum(two) to enable the test suite
        XDo::XWindow.desktop_num = 2
        @reset_num_workspaces = true
    end

    $SUT.execute_shell_command 'killall xman'
    @@current_workspace = XDo::XWindow.desktop
    @@xid_list=Array.new

  end
  
  # Run once at the end of this test suite
  shutdown do
    if @reset_num_workspaces == true
        # Resetting number of workspaces to " + @number_of_workspaces.to_s
        XDo::XWindow.desktop_num=@number_of_workspaces
    end
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @app = $SUT.run( :name => UNITY_2D_SHELL,
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
    @@title_string = ""
  end

  # Run after each test case completes
  teardown do
    close_all_test_windows
    reset_current_workspace
    system "pkill -nf unity-2d-shell"
  end

    #####################################################################################
    # Test cases

    # Test case objectives:
    #   * Check pips are updated properly when the app is in current workspace
    # Pre-conditions
    #   * This test case assumes that the test app will have only one window
    #   * For eg, xman is choosen
    #   * To ensure there is no xman windows running, kill all the xman windows if any
    # Test steps
    #   * Open a xman window
    #   * Verify the launcher tile pip image matches with the expected one
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check pips are updated properly when the app is in current workspace" do
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
    # Pre-conditions
    #   * None
    # Test steps
    #   * Move the window to a different workspace
    #   * Verify the launcher tile pip image matches with the expected one
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check pips are updated properly when the app is moved to different workspace" do
        xid = open_window()
        @@xid_list << xid

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
    #   * Check launcher pips to indicate an app with two windows each on two different workspaces
    # Pre-conditions
    #   * None
    # Test steps
    #   * Open another window along with first window from the first testcase.
    #   * Verify the launcher tile pip images matches with the expected one
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check launcher pips to indicate an app with two windows each on two different workspaces" do
        xid = open_window()
        @@xid_list << xid
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
    #   * Check launcher pips to indicate an app completely belonging to different workspace
    # Pre-conditions
    #   * None
    # Test steps
    #   * Move the second window too to a different workspace
    #   * Verify the launcher tile pip image matches with the expected one
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check launcher pips to indicate an app completely belonging to different workspace" do
        xid = open_window()
        @@xid_list << xid
        change_window_workspace(xid)

        xid = open_window()
        @@xid_list << xid
        change_window_workspace(xid)

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching expected image launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end


    # Test case objectives:
    #   * Check launcher pips when an app window is closed.
    # Pre-conditions
    #   * None
    # Test steps
    #   * Close one of the app windows and move the last window back to its original workspace
    #   * Verify the launcher tile pip image matches with the expected one
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check launcher pips when an app window is closed" do
        xid = open_window()
        @@xid_list << xid
        xid = open_window()
        @@xid_list << xid

        xid.close!
        change_window_workspace(@@xid_list[0])

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching expected image launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end


    # Test case objectives:
    #   * Check launcher pips to indicate when the user workspace changed
    # Pre-conditions
    #   * None
    # Test steps
    #   * Change the user workspace to other than app workspace
    #   * Verify that the pips are updated properly
    # Post-conditions
    #   * None
    # References
    #   * lp:#883172
    test "Check launcher pips to indicate when the user workspace changed" do
        xid = open_window()
        @@xid_list << xid

        change_user_workspace

        expected_pip_image = 'image://blended/launcher/artwork/launcher_arrow_outline_ltr.pngcolor=lightgreyalpha=1'

        verify_equal(expected_pip_image, TIMEOUT, 'pip not matching expected image launcher_arrow_outline_ltr.png'){
             @app.Launcher() \
            .LauncherList( :name => 'main' ) \
            .QDeclarativeItem( :name => @@title_string ) \
            .QDeclarativeImage( :name => 'pips-0')['source']
        }
    end
end
