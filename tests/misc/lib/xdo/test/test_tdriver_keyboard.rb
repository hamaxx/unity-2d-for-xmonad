#!/usr/bin/env ruby
#Encoding: UTF-8

require "test/unit"

# Run XDo test suite through Testability Driver
require "tdriver"
require "timeout"
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
    window_id = -1
    XDo.sut = TDriver.sut(:Id => "sut_qt") # the magic to use tdriver
    XDo.sut.execute_shell_command(EDITOR_CMD, :detached => true)
    Timeout.timeout(10){ window_id = XDo::XWindow.wait_for_window(EDITOR_NAME)}
    Kernel.raise(SystemCallError, "Unable to open or find #{EDITOR_NAME}") if window_id == -1
    @xid = XDo::XWindow.new(window_id)
    sleep 1
  end
  
  def teardown
    @xid.close!
  end

  def test_char
    window_id = -1
    @xid.close!
    #Special file need to be opened
    tempfile = Tempfile.open("XDOTEST")
    tempfile.write(TESTTEXT)
    tempfile.flush
    sleep 3 #Wait for the buffer to be written out
    #Copy file to SUT
    tempfilename = File.basename(tempfile.path)
    XDo.sut.copy_to_sut(:file => tempfile.path, :to => '/var/tmp')
    XDo.execute(EDITOR_CMD + ' /var/tmp/' + tempfilename)
    Timeout.timeout(30){ window_id = XDo::XWindow.wait_for_window(EDITOR_NAME)}
    Kernel.raise(SystemCallError, "Unable to open or find #{EDITOR_NAME}") if window_id == -1
    @xid = XDo::XWindow.new(window_id)
    sleep 1
    20.times{XDo::Keyboard.char("Shift+Right")}
    XDo::Keyboard.ctrl_c
    sleep 0.2
    assert_equal(TESTTEXT, XDo::Clipboard.read_clipboard)
    tempfile.close
    XDo.execute('rm /var/tmp/' + tempfilename)
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
    @xid.unfocus #Ensure that the editor hasn't the input focus anymore
    sleep 1
    XDo::Keyboard.simulate(TESTTEXT_SPECIAL, false, @xid)
    sleep 1
    @xid.activate
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
