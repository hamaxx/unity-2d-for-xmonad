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

require 'xdo/xwindow'
require 'xdo/keyboard'
require 'timeout'
require 'tmpdir'

# Helper class to open temporary windows and clean them up after
class TmpWindow

  #Open window with top left corner at certain position
  #===Parameters
  #[+x+] (100) x coordinate of the top left corner of the tempoaray window
  #[+y+] (100) y coordinate of the top left corner of the tempoaray window
  #===Return value
  # XDo::XWindow id of the temporary window
  #===Raises
  #[SystemCallError] Error executing +gnome-terminal+.
  #===Example
  #  TmpWindow.open_window_at(200, 300)
  def self.open_window_at(x=100,y=100)
    window_id = -1
    # Generate a temporary directory (used to generate unique window title to help Xdo get window ID)
    t = Time.now.strftime("%Y%m%d")
    path = "/tmp/#{t}-#{$$}-#{rand(0x100000000).to_s(36)}"
    $SUT.execute_shell_command('mkdir -p ' + path)
    # Open Terminal with position (x,y)
    $SUT.execute_shell_command("gnome-terminal --geometry=100x30+#{x}+#{y} --working-directory=#{path}", :detached => true)
    Timeout.timeout(30){ window_id = XDo::XWindow.wait_for_window(path)}
    Kernel.raise(SystemCallError, "Unable to open gnome-terminal") if window_id == -1
    xid = XDo::XWindow.new(window_id)
    # create list of windows opened
    @xid_list = [] if @xid_list == nil
    @xid_list << xid
    # remove temporary directory
    $SUT.execute_shell_command('rmdir ' + path)
    xid
  end

  #Close all windows opened
  #===Return value
  # nil
  #===Raises
  #[XError] kill! fails to execute +xkill+.
  #===Example
  #  TmpWindow.close_all_windows
  def self.close_all_windows
    return if @xid_list.nil?
    # loop through list of opened windows, check if they exist, try close then kill
    @xid_list.each do |xid|
      need_to_kill = true
      if xid.exists?
        window_title = xid.title
        xid.close!
        Timeout.timeout(10){ 
          XDo::XWindow.wait_for_close(window_title)
          need_to_kill = false
        }
        xid.kill! if need_to_kill
      end
    end
    @xid_list = []
  end
end
