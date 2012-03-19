=begin
/*
 * This file is part of unity-2d
 *
 * Copyright 2012 Canonical Ltd.
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

# Test case objectives:
#   * Check that the hint is displayed on mouse hover
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Hover the cursor over the first application entry
#   * Check that the hint is displayed
#   * Check that there are two QActions (there is a "ghost" QAction there)
# Post-conditions
#   * None
# References
#   * None
def test_display_launcher_item_hint()
  tiles = ""
  verify( TIMEOUT, 'Could not find any application tile' ) {
    tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
  }
  tile = tiles[0]
  tile.move_mouse()
  verify(TIMEOUT, 'The launcher item hint is not visible' ) {
    @app.LauncherContextualMenu()
  }
  actions = ""
  verify( TIMEOUT, 'Could not find any actions in the menu' ) {
    actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
  }
  verify_equal( 2, TIMEOUT, 'There was an unexpected number of actions in the menu' ) {
    actions.count
  }
end

# Test case objectives:
#   * Check that the menu is displayed on right click
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Right click the first application entry
#   * Check that the menu is displayed
#   * Check that there are at least four QActions (two actions and separator, plus a "ghost" one)
# Post-conditions
#   * None
# References
#   * None
def test_display_launcher_menu_after_right_click()
  tiles = ""
  verify( TIMEOUT, 'Could not find any application tile' ) {
    tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
  }
  tile = tiles[0]
  tile.move_mouse()
  tile.tap(1, :Right)
  verify(TIMEOUT, 'The launcher menu is not visible' ) {
    @app.LauncherContextualMenu()
  }
  actions = ""
  verify( TIMEOUT, 'Could not find any actions in the menu' ) {
    actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
  }
  verify_true( TIMEOUT, 'There was not enough actions in the menu' ) {
    actions.count >= 4
  }
end

# Test case objectives:
#   * Check that the menu is displayed on right key from item
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go down and right
#   * Check that the menu is displayed
#   * Check that there are at least four QActions (two actions and separator, plus a "ghost" one)
# Post-conditions
#   * None
# References
#   * None
def test_display_launcher_menu_with_keyboard_navigation()
  XDo::Keyboard.alt_F1
  XDo::Keyboard.down
  keyboard_tap_right()
  verify(TIMEOUT, 'The launcher menu is not visible' ) {
    @app.LauncherContextualMenu()
  }
  actions = ""
  verify( TIMEOUT, 'Could not find any actions in the menu' ) {
    actions = @app.LauncherContextualMenu().children( { :type => "QAction" } )
  }
  verify_true( TIMEOUT, 'There was not enough actions in the menu' ) {
    actions.count >= 4
  }
end

# Test case objectives:
#   * Check that the menu is being closed on Esc
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go down and right, press Esc
#   * Check that the menu is no longer there
# Post-conditions
#   * None
# References
#   * None
def test_close_launcher_menu_when_pressing_esc()
  XDo::Keyboard.alt_F1
  XDo::Keyboard.down
  keyboard_tap_right()
  XDo::Keyboard.escape
  verify_not(TIMEOUT, 'The launcher menu is not visible' ) {
    @app.LauncherContextualMenu()
  }
end

# Test case objectives:
#   * Check that the menu is being closed on left key
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go down and right, then left
#   * Check that the menu is no longer there
# Post-conditions
#   * None
# References
#   * None
def test_close_launcher_menu_when_navigating_back_to_the_launcher()
  XDo::Keyboard.alt_F1
  XDo::Keyboard.down
  keyboard_tap_right()
  keyboard_tap_left()
  verify_not(TIMEOUT, 'The launcher menu is not visible' ) {
    @app.LauncherContextualMenu()
  }
end

# Test case objectives:
#   * Check that the focus goes back to the launcher item when menu was dismissed with Esc
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go down and right, press Esc
#   * Check that the launcher item has focus
# Post-conditions
#   * None
# References
#   * None
def test_verify_launcher_tile_gets_focus_after_dismissing_the_menu_with_esc()
  XDo::Keyboard.alt_F1
  XDo::Keyboard.down
  keyboard_tap_right()
  XDo::Keyboard.escape
  tiles = ""
  verify( TIMEOUT, 'Could not find any application tile' ) {
    tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
  }
  tile = tiles[0]
  verify_equal( "true", TIMEOUT, 'Launcher item didn\'t regain focus' ) {
    tile['activeFocus']
  }
end

# Test case objectives:
#   * Check that the focus goes back to the launcher item when menu was dismissed with keyboard navigation
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go down and right, press Esc
#   * Check that the launcher item has focus
# Post-conditions
#   * None
# References
#   * None
def test_verify_launcher_tile_gets_focus_after_dismissing_the_menu_with_keyboard_navigation()
  XDo::Keyboard.alt_F1
  XDo::Keyboard.down
  keyboard_tap_right()
  keyboard_tap_left()
  tiles = ""
  verify( TIMEOUT, 'Could not find any application tile' ) {
    tiles = @app.LauncherList( :name => 'main' ).children( { :desktopFile => /^.*.desktop$/ } )
  }
  tile = tiles[0]
  verify_equal( "true", TIMEOUT, 'Launcher item didn\'t regain focus' ) {
    tile['activeFocus']
  }
end

# Test case objectives:
#   * Check that the focus goes from launcher menu to dash
# Pre-conditions
#   * Desktop with no running applications
# Test steps
#   * Focus the launcher, go right, press Super
#   * Check that the dash search entry has focus
# Post-conditions
#   * None
# References
#   * None
def test_verify_dash_search_entry_gets_focus_after_dismissing_the_menu_with_super()
  XDo::Keyboard.alt_F1
  keyboard_tap_right()
  XDo::Keyboard.super
  verify_equal( "true", TIMEOUT, 'Dash search entry doesn\'t have focus' ) {
    @app.SearchEntry()['activeFocus']
  }
end

