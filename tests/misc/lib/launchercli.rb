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

# Helper class to interact with Launcher items.
class LauncherItemClient

    # fake_progress.py location relative to the directory of current script
    @@FAKE_PROGRESS_SCRIPT = File.dirname(__FILE__) + '/../fake_progress.py'

    def initialize(desktop_file)
        @desktop_file = desktop_file
    end

    def set_progress(value)
        `#{@@FAKE_PROGRESS_SCRIPT} -t #{@desktop_file} #{value}`
    end

    def hide_progress()
        `#{@@FAKE_PROGRESS_SCRIPT} -f #{@desktop_file}`
    end
end
