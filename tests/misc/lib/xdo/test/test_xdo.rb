#!/usr/bin/env ruby
#Encoding: UTF-8

require "test/unit"
# The following line includes the complete tdriver environment 
require 'tdriver'

require File.join(File.dirname(__FILE__), '../_xdo')

class XdoTest < Test::Unit::TestCase

def setup
end

def teardown
end

def test_host_success
  return_code, stdout, stderr = XDo.execute('ls /')
  assert return_code == 0
  assert stderr.empty?
end

def test_host_fail
  return_code, stdout, stderr = XDo.execute('ls /wrong')
  assert return_code == 2
  assert stdout.empty?
end

def test_tdriver_success
  XDo.sut = TDriver.sut(:Id => "sut_qt")
  return_code, stdout, stderr = XDo.execute('ls /')
  assert return_code == 0
  assert stderr.empty?
  XDo.sut = nil
end

def test_tdriver_fail
  XDo.sut = TDriver.sut(:Id => "sut_qt")
  return_code, stdout, stderr = XDo.execute('ls /wrong')
  assert return_code == 2
  assert stdout.empty?
  XDo.sut = nil
end

def test_host_tdriver_equal
  host_return_code, host_stdout, host_stderr = XDo.execute('ls /wrong')
  XDo.sut = TDriver.sut(:Id => "sut_qt")
  sut_return_code, sut_stdout, sut_stderr = XDo.execute('ls /wrong')
  assert_equal(host_return_code, sut_return_code)
  assert_equal(host_stdout, sut_stdout)
  assert_equal(host_stderr, sut_stderr)
end

def test_host_tdriver_equal
  host_return_code, host_stdout, host_stderr = XDo.execute('echo "hello"')
  XDo.sut = TDriver.sut(:Id => "sut_qt")
  sut_return_code, sut_stdout, sut_stderr = XDo.execute('echo "hello"')
  assert_equal(host_return_code, sut_return_code)
  assert_equal(host_stdout, sut_stdout)
  assert_equal(host_stderr, sut_stderr)
end
end
