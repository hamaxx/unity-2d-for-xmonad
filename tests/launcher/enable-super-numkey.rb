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
require 'xdo/keyboard'

############################# Test Suite #############################
context "Launcher Super+NUM key support Tests" do
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
    @app = @sut.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability",
                     :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    #@app.close
    #Need to kill Launcher as it does not shutdown when politely asked
    $SUT.execute_shell_command "pkill -nf unity-2d-launcher"
  end

  #####################################################################################
    # Test cases
    # Test case objectives:
    # * To verify Launcher tile shortcuts Super+NumKey[0,9] are enabled
    # Pre-conditions
    # * None
    # Test steps
    # * List the numeric shortcuts from LauncherList
    # * Simluate the Super+Numkey[0,9].
    # * Verify that corresponding application is running.
    # Post-conditions
    # * None
    # References
    # * lp: 750514
    test "Launcher enable Super+Numkey for tile shortcuts" do
        tile_list = @app.Launcher().LauncherList(:name =>'main').children ({ :shortcutText => /\d/ })
        tile_list.delete_if {|x| tile_list.index(x) >= 10 } #Remove duplicates if any.
        tile_list.each do |tile|
            shortcut = tile["shortcutText"]
            numkey="{NUM"+shortcut+"}"
            XDo::Keyboard.key_down('SUPER')
            XDo::Keyboard.simulate(numkey)
            XDo::Keyboard.key_up('SUPER')

            verify_true(TIMEOUT, numkey+"-Failed") {
                tile['running'] == 'true'
            }
        end
    end
end
