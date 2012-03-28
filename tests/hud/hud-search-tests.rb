#!/usr/bin/env ruby1.8
=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
 *
 * Authors:
 * - Pawel Stolowski <pawel.stolowski@canonical.com>
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

HUD_HIGHLIGHT_COLOR = '#ffffff'
HUD_NO_HIGHLIGHT_COLOR = '#000000'
HUD_RESULTS_START_Y = 53
HUD_RESULT_ITEM_HEIGHT = 42

######################### Helper functions ############################

#
# Returns array of ResultItem objects
def get_search_results(shell)
    return shell.Hud().QDeclarativeItem().QDeclarativeRectangle().QDeclarativeListView().QDeclarativeItem().children({:type=>:ResultItem}, false)
end

#
# Verify that given ResultItem is highlighted and active.
def item_is_highlighted(results, index)
    verify_equal('true', TIMEOUT, "Result item ##{index+1} should have focus") {
        results[index]['activeFocus']
    }

    verify_equal(HUD_HIGHLIGHT_COLOR, TIMEOUT, "Result item ##{index+1} should be highlighted") {
        results[index].QDeclarativeRectangle(:name=>'container')['color']
    }
end

#
# Verify that exactly one item is highlighted and active.
def only_one_item_highlighted(results)
    verify_equal(1, TIMEOUT, 'Exactly one item should be highlighted') {
        results.count {|x| x.QDeclarativeRectangle(:name=>'container')['color'] == HUD_HIGHLIGHT_COLOR}
    }

    verify_equal(1, 0, 'Exactly one item has focus') {
        results.count {|x| x['activeFocus'] == 'true'}
    }

    verify_equal(1, 0, "Exactly one item has 'selected' state ") {
        results.count {|x| x['state'] == 'selected'}
    }
end

############################# Test Suite #############################
context "HUD Search tests" do

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
    @app = $SUT.run( :name => UNITY_2D_SHELL,
                     :arguments => "-testability", 
                     :sleeptime => 2 )
    # Make certain application is ready for testing
    verify{ @app.LauncherLoader() }

    # Move mouse out of way
    XDo::Mouse.move(500, 500, 0, true)
  end

  # Run after each test case completes
  teardown do
    #Need to kill Shell as it does not shutdown when politely asked
    $SUT.execute_shell_command 'pkill -nf unity-2d-shell'
  end

  #####################################################################################
  # Test cases

  # Test case objectives:
  #   * Check that first item is highlighted by default in HUD search results
  # Pre-conditions
  #   * None
  # Test steps
  #   * Tap Alt to open HUD
  #   * Check that HUD is visible
  #   * Type search string (letter 'a')
  #   * Verify that exactly 5 result items are displayed
  #   * Verify that first result item is highlighted and has focus
  #   * Verify that exactly one result item is highlighted
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/948441
  test "First result should be highlighted by default" do
    # wait for hud to become ready
    sleep(1);

    XDo::Keyboard.alt

    verify_equal('true', TIMEOUT, 'HUD should be visible') {
      @app.Hud()['active']
    }

    # type search string
    XDo::Keyboard.a

    results = get_search_results(@app)

    verify_equal(6, 0, 'Should get exactly 6 rows') {
        results.length
    }

    item_is_highlighted(results, 0)
    only_one_item_highlighted(results)
  end

  # Test case objectives:
  #   * Check that keyboard arrows change highlighted item in HUD search results
  # Pre-conditions
  #   * None
  # Test steps
  #   * Tap Alt to open HUD
  #   * Check that HUD is visible
  #   * Type search string (letter 'a')
  #   * Press 'down'
  #   * Verify that second result item is highlighted and has focus
  #   * Verify that exactly one result item is highlighted
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/948441
 test "Highlight should follow keyboard" do
    # wait for hud to become ready
    sleep(1);

    XDo::Keyboard.alt

    verify_equal('true', TIMEOUT, 'HUD should be visible') {
      @app.Hud()['active']
    }

    # type search string
    XDo::Keyboard.a

    verify(TIMEOUT, 'Results list should be visible') {
        get_search_results(@app)
    }

    XDo::Keyboard.down

    results = get_search_results(@app)
    item_is_highlighted(results, 1)
    only_one_item_highlighted(results)
  end

  # Test case objectives:
  #   * Check that moving mouse over HUD search results moves highlight
  # Pre-conditions
  #   * None
  # Test steps
  #   * Tap Alt to open HUD
  #   * Check that HUD is visible
  #   * Type search string (letter 'a')
  #   * Move mouse over second results item
  #   * Verify that second result item is highlighted and has focus
  #   * Verify that exactly one result item is highlighted
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/948441
  test "Highlight should follow mouse" do
    # wait for hud to become ready
    sleep(1);

    XDo::Keyboard.alt

    verify_equal('true', TIMEOUT, 'HUD should be visible') {
      @app.Hud()['active']
    }

    # type search string
    XDo::Keyboard.a

    verify(TIMEOUT, 'Results list should be visible') {
        get_search_results(@app)
    }

    # Move mouse over second item
    XDo::Mouse.move(100, HUD_RESULTS_START_Y + HUD_RESULT_ITEM_HEIGHT+ 2, 0, true)

    results = get_search_results(@app)

    item_is_highlighted(results, 1)
    only_one_item_highlighted(results)
  end

  # Test case objectives:
  #   * Check that moving mouse over 2nd item and then pressing 'Down' key moves highlight to 3rd item
  # Pre-conditions
  #   * None
  # Test steps
  #   * Tap Alt to open HUD
  #   * Check that HUD is visible
  #   * Type search string (letter 'a')
  #   * Move mouse over second results item
  #   * Verify that second result item is highlighted and has focus
  #   * Verify that exactly one result item is highlighted
  #   * Press 'down'
  #   * Verify that second result item is highlighted and has focus
  #   * Verify that exactly one result item is highlighted
  # Post-conditions
  #   * None
  # References
  #   * https://bugs.launchpad.net/unity-2d/+bug/948441
  test "Mixing Keyboard & Mouse navigation should highlight only one item" do
    # wait for hud to become ready
    sleep(1);

    XDo::Keyboard.alt

    verify_equal('true', TIMEOUT, 'HUD should be visible') {
      @app.Hud()['active']
    }

    # type search string
    XDo::Keyboard.a

    verify(TIMEOUT, 'Results list should be visible') {
        get_search_results(@app)
    }

    # Move mouse over second item
    XDo::Mouse.move(100, HUD_RESULTS_START_Y + HUD_RESULT_ITEM_HEIGHT+ 2, 0, true)

    results = get_search_results(@app)

    item_is_highlighted(results, 1)
    only_one_item_highlighted(results)

    verify(TIMEOUT, 'Results list should be visible') {
        get_search_results(@app)
    }

    XDo::Keyboard.down

    item_is_highlighted(results, 2)
    only_one_item_highlighted(results)
  end
end
