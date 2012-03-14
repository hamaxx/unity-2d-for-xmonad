#!/usr/bin/python

#
# Simple script to fake progress updates on Unity Launcher items.
# Run without arguments for usage.
#
# Author: Pawel Stolowski <pawel.stolowski@canonical.com>
#

from gi.repository import Unity, GObject
import sys

usage = """Usage:
fake_progress.py [-t|-f] <app> [value]

  -t    Show progress_visible = true property
  -f    Send progress_visible = false property
  app   Name of application .desktop file on the Launcher (e.g. "nautilus-home.desktop")
  value Progress bar value (normally 0.0 - 1.0, but invalid values may be used for testing)
"""

loop = GObject.MainLoop()
    
def force_quit():
    loop.quit()
 
send_show = False
send_hide = False

i  = 1
if len(sys.argv) > i:
    if sys.argv[i] == '-f':
        send_hide = True
        i = i+1
    if sys.argv[i] == '-t':
        send_show = True
        i = i+1

ctx = GObject.MainContext()
if len(sys.argv) > i:
    appname = sys.argv[i]
    i = i+1
    launcher = Unity.LauncherEntry.get_for_desktop_id(appname)
    
    if len(sys.argv) > i:
        progress = float(sys.argv[i])
        print 'send: progress = ' + sys.argv[i]
        launcher.set_property("progress", progress)

    if send_show:
        print 'send: progress_visible = true'
        launcher.set_property("progress_visible", True)

    if send_hide:
        print 'send: progress_visible = false'
        launcher.set_property("progress_visible", False)
else:
    sys.exit(usage)

GObject.timeout_add(100, force_quit)
loop.run()
