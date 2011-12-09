#!/usr/bin/env ruby
#
# Testability test suite execution script
# 
# Run all tests with
#   $ ruby run-tests.rb

# Add ./misc/lib to the list of library locations - need to calculate absolute path
require 'pathname'
$library_path = File.dirname(Pathname.new(File.expand_path(__FILE__)).realpath) + '/misc/lib'
$LOAD_PATH.unshift $library_path

# The following line includes the complete tdriver environment 
require 'tdriver'
include TDriverVerify

# Require unit test framework: This enables execution of test cases and also includes assertions (Test::Unit::Assertions)
require 'testhelper'

# List of directories in which to search for test cases
test_directories = ['launcher', 'panel', 'places', 'spread', 'window-manager', 'other']

# Only run scan for tests if this script is directly called
if __FILE__ == $0
    $INIT_COMPLETED = true # Prevent this file being included by test cases

    # Scan through the above directories and execute test cases contained.
    test_directories.each do | directory |
        Dir["#{directory}/*.rb"].each { |testCase| require testCase}
    end
end
