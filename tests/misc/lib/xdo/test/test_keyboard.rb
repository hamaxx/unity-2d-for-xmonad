#!/usr/bin/env ruby
#Encoding: UTF-8

require "test/unit"
require "tempfile"
require File.join(File.dirname(__FILE__), '../keyboard')
require File.join(File.dirname(__FILE__), '../clipboard')
require File.join(File.dirname(__FILE__), '../xwindow')
require File.join(File.dirname(__FILE__), '../simulatable')

class TestKeyboard < Test::Unit::TestCase
  
  #Command to start a simple text editor
  EDITOR_CMD = "gedit -s"
  EDITOR_NAME = "gedit"
  
  TESTTEXT = "This is test\ntext."
  TESTTEXT2 = "WXY"
  TESTTEXT_RAW = "ä{TAB}?b"
  TESTTEXT_SPECIAL = "ab{TAB}c{TAB}{TAB}d"
  
  def setup
    @editor_pipe = IO.popen(EDITOR_CMD, 'r')
    sleep 3
  end
  
  def teardown
    Process.kill 'TERM', @editor_pipe.pid
    @editor_pipe.close
  end

  def test_char
    Process.kill 'TERM', @editor_pipe.pid
    @editor_pipe.close #Special file need to be opened
    tempfile = Tempfile.open("XDOTEST")
    tempfile.write(TESTTEXT)
    tempfile.flush
    sleep 3 #Wait for the buffer to be written out
    @editor_pipe = IO.popen(EDITOR_CMD + ' ' + tempfile.path, 'r') #So it's automatically killed by #teardown
    sleep 3
    tempfile.close
    20.times{XDo::Keyboard.char("Shift+Right")}
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT, XDo::Clipboard.read_clipboard)
  end
  
  def test_simulate
    XDo::Keyboard.simulate("A{BS}#{TESTTEXT2}")
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT2, XDo::Clipboard.read_clipboard)
    
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.delete
    XDo::Keyboard.simulate(TESTTEXT_SPECIAL)
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT_SPECIAL.gsub("{TAB}", "\t"), XDo::Clipboard.read_clipboard)
    
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.delete
    XDo::Keyboard.simulate(TESTTEXT_RAW, true)
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT_RAW, XDo::Clipboard.read_clipboard)
  end
  
  def test_type
    XDo::Keyboard.type(TESTTEXT2)
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT2, XDo::Clipboard.read_clipboard)
  end

=begin #Disabling this failing test due to poor window Xid detection by xdotool - fix on the way
  def test_window_id
    XDo::XWindow.unfocus #Ensure that the editor hasn't the input focus anymore
    sleep 1
    edit_id = XDo::XWindow.search(EDITOR_NAME).last
    xwin = XDo::XWindow.new(edit_id)
    XDo::Keyboard.simulate(TESTTEXT_SPECIAL, false, edit_id)
    sleep 1
    xwin.activate
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT_SPECIAL.gsub("{TAB}", "\t"), XDo::Clipboard.read_clipboard)
  end
=end
  
  def test_include
    String.class_eval do
      include XDo::Simulatable
      
      def to_xdo
        to_s
      end
    end
    
    XDo::Keyboard.ctrl_a
    XDo::Keyboard.delete
    "Ein String".simulate
    XDo::Keyboard.ctrl_a
    sleep 0.2
    XDo::Keyboard.ctrl_c
    sleep 0.2
    clip = XDo::Clipboard.read_clipboard
    assert_equal("Ein String", clip, "Simulated typed string fails to match")
  end
  
end
