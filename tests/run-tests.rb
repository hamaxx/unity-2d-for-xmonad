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

# If cmake was called, obtain the path to the built binary directory. If not, we test the
# installed applications instead
binary_dir_file = $library_path + '/../binary_dir.txt'
if File.exists?(binary_dir_file)
    binary_dir = File.open(binary_dir_file).first.strip
    puts 'Running tests on applications contained within ' + binary_dir
    UNITY_2D_LAUNCHER = binary_dir + '/launcher/app/unity-2d-launcher'
    UNITY_2D_PANEL = binary_dir + '/panel/app/unity-2d-panel'
    UNITY_2D_PLACES = binary_dir + '/places/app/unity-2d-places'
    UNITY_2D_SPREAD = binary_dir + '/spread/app/unity-2d-spread'
else
    puts 'NOTICE: source not configured, tests will be carried out on *installed* applications!'
    UNITY_2D_LAUNCHER = 'unity-2d-launcher'
    UNITY_2D_PANEL = 'unity-2d-panel'
    UNITY_2D_PLACES = 'unity-2d-places'
    UNITY_2D_SPREAD = 'unity-2d-spread'
end

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
