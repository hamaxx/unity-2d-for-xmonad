#Encoding: UTF-8
#This file is part of Xdo. 
#Copyright © 2009 Marvin Gülker
#  Initia in potestate nostra sunt, de eventu fortuna iudicat. 
#
# This file contains shared definitions for all the xdo scripts
#
# Modified by Gerry Boland <gerry dot boland at canonical dot com>

require "open3"
require "strscan"

#The namespace of this library. 
module XDo
  
  #The command to start xdotool. 
  XDOTOOL = "xdotool"
  
  #The command to start xsel. 
  XSEL = "xsel"
  
  #The command to start xwininfo. 
  XWININFO = "xwininfo"
  
  #The command to start xkill. 
  XKILL = "xkill"

  #The command to query gconf
  GCONFTOOL = "gconftool-2"
  
  #Class for errors in this library. 
  class XError < StandardError
  end
  
  class ParseError < StandardError
  end
  
end #module XDo
