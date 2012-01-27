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

  #TDriver SUT connection
  def self.sut=(input)
    @sut = input
  end

  def self.sut
    @sut
  end

  # Execute a shell command. If :sut is set, Testability Driver is used to execute
  # the command on the system under test. Else is run on current machine.
  def self.execute(cmd, timeout=3)
    stdout = ''
    stderr = ''
    return_code = -1

    if @sut == nil
      # Running command in shell. Open3 doesn't allow easy access to return code, so let's use a little bash-fu
      stdin, out, err = Open3.popen3(cmd + '; echo "\n$?"')
      stdout = out.read.strip
      stderr = err.read.strip

      #remove last line of stderr and get return code from it
      return_code = stdout.split("\n")[stdout.lines.count-1].to_i
      stdout.chomp!(return_code.to_s)
      stdout.chomp!("\n")
    else
      # Tdriver has the shell_command method to extract stdout, stderr and return number from a shell command
      # run with execute_shell_command. However it requires the shell command to be detached and placed
      # in a separate thread. In my experiments execution and return takes about 2 seconds, whereas a simple 
      # synchroneous execution takes mere miliseconds. This wrapper is a bit of bash-fu to split stdout
      # and stderr and obtain the return code while executing it synchroneously. We run new bash instance as 
      # command is run in a QProcess which takes all stdout and stderr output from us.
      #
      # To explain the Bash-fu, reading from the outside in (ref: http://tinyurl.com/6ndfm8d):
      # - creates a file descriptor $out for the whole block, duplicating stdout
      # - captures the stdout of the whole command in $error (but see below)
      # - the command itself redirects stderr to stdout (which gets captured above) then stdout to the original 
      #   stdout from outside the block, so only the stderr gets captured

      wrap_cmd = 'bash -c "{ error=$(' + cmd + ' 2>&1 1>&$out); } {out}>&1; echo -n \"==return-code:$?==${error}\""'

      # Bug workaround: the command is sent to the qttasserver via xml. The command isn't escaped by
      # execute_system_command. Need to escape certain chars of the command ourselves.
      wrap_cmd.gsub!("&", "&amp;")
      wrap_cmd.gsub!("<", "&lt;")
      wrap_cmd.gsub!(">", "&gt;")

      output = @sut.execute_shell_command(wrap_cmd, :timeout => timeout)
      #Now parse output
      errors_exist = false

      output.strip.each_line do |line|

        #look for "==return-code:"
        if line.match( /^(.*)==return-code:(\d+)==(.*)/ ) and errors_exist == false
          return_code = $2.to_i
          errors_exist = true
          stdout.concat($1)
          stderr.concat($3)
          next
        end

        stderr.concat(line) if errors_exist
        stdout.concat(line) unless errors_exist
      end

      stderr.strip!
    end

    return return_code, stdout, stderr
  end

end #module XDo
