require 'test/unit'

dir = File.dirname(File.expand_path(__FILE__))
$LOAD_PATH.unshift dir + '/../lib'
$TEST_DIR = File.dirname(File.expand_path(__FILE__))

##
# Test::Unit runs test in alphabetical order. This class instead runs them
# sequentially. Can specify ordering with the line
# execute :sequentially:
# Taken from http://wiki.openqa.org/download/attachments/804/testcase.rb
# Copyright: Bret Pettichord

class TestCase < Test::Unit::TestCase
  @@order = :sequentially
  def initialize name
    throw :invalid_test if name == :default_test && self.class == TestCase
    super
  end        
  class << self
    attr_accessor :test_methods, :order
    def test_methods
      @test_methods ||= []
    end
    def order
      @order || @@order
    end
    def default_order= order
      @@order = order
    end
    def sorted_test_methods
      case order
      when :alphabetically:          test_methods.sort
      when :sequentially:            test_methods
      when :reversed_sequentially:   test_methods.reverse
      when :reversed_alphabetically: test_methods.sort.reverse
      else raise ArgumentError, "Execute option not supported: #{@order}"
      end
    end
    def suite
      suite = Test::Unit::TestSuite.new(name)
      sorted_test_methods.each do |test|
        catch :invalid_test do
          suite << new(test)
        end
      end
      if (suite.empty?)
        catch :invalid_test do
          suite << new(:default_test)
        end
      end
      return suite
    end
    def method_added id
      name = id.id2name
      test_methods << name if name =~ /^test./
    end
    def execute order
      @order = order
    end
  end
end


##
# Snippit from test/spec/mini 5
# Allows syntatic sugar for tests in the following form:
#
#context "It's test/spec/mini!" do
#  setup do
#    @name = "Chris"
#  end
#
#  teardown do
#    @name = nil
#  end
#
#  test "with Test::Unit" do
#    assert (self.class < Test::Unit::TestCase)
#  end
#
#  test "body-less test cases"
#
#  test :symbol_test_names do
#    assert true
#  end
#
#  xtest "disabled tests" do
#    assert disabled!
#  end
#
#  context "and of course" do
#    test "nested contexts!" do
#      assert_equal "Chris", @name
#    end
#  end
#end
#
# http://gist.github.com/307649 (chris@ozmm.org)
#

def context(*args, &block)
  return super unless (name = args.first) && block

  klass = Class.new(TestCase) do
    def self.test(name, &block)
      define_method("test_#{name.to_s.gsub(/\W/,'_')}", &block) if block
    end
    def self.xtest(*args) end
    def self.context(*args, &block) instance_eval(&block) end
    def self.setup(&block)
      define_method(:setup) { self.class.setups.each { |s| instance_eval(&s) } }
      setups << block
    end
    def self.setups; @setups ||= [] end
    def self.teardown(&block) define_method(:teardown, &block) end
  end
  (class << klass; self end).send(:define_method, :name) { name.gsub(/\W/,'_') }
  klass.class_eval &block
end
