#!/usr/bin/env ruby
#
# Testability test suite execution script
# 
# Run all tests with
#   $ ruby run-tests.rb

# Add ./misc/lib to the list of library locations
$LOAD_PATH.unshift File.expand_path('./misc/lib')

require 'testhelper'

# List of directories in which to search for test cases
test_directories = ['launcher', 'panel', 'places', 'spread', 'window-manager', 'other']

# Scan through the above directories and execute test cases contained.
test_directories.each do | directory |
    Dir["#{directory}/*.rb"].each { |testCase| require testCase}
end
