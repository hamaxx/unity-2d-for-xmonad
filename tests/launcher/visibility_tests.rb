require '../run-tests.rb' unless $INIT_COMPLETED
require 'xdo/xwindow'
require 'xdo/keyboard'
require 'xdo/mouse'
require 'timeout'

# Helper function to open window at certain position
def open_window_at(x,y)
  # Open Terminal with position (x,y)
  system "gnome-terminal --geometry=100x30+#{x}+#{y} --working-directory=/dev/disk/"
  Timeout.timeout(3){XDo::XWindow.wait_for_window("gnome-terminal")}
  return XDo::XWindow.from_active
end


context "Launcher Visibility Tests" do
  # Run once at the beginning of this test suite
  startup do
    system 'killall unity-2d-launcher > /dev/null 2>&1'
    system 'killall unity-2d-launcher > /dev/null 2>&1'

    # Minimize all windows
    XDo::XWindow.toggle_minimize_all
  end
  
  # Run once at the end of this test suite
  shutdown do
  end

  # Run before each test case begins
  setup do
    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")    
    @app = @sut.run( :name => UNITY_2D_LAUNCHER, 
    		         :arguments => "-testability", 
    		         :sleeptime => 2 )
  end

  # Run after each test case completes
  teardown do
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-launcher"
  end

  #####################################################################################
  # Test cases
  test "Position with Empty Desktop" do
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher hiding on empty desktop, should be visible' )
  end

  test "Position with Window not in the way" do
    # Open Terminal with position 100x100
    xid = open_window_at(100,100)
    sleep 0.5
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher hiding when window not in the way, should be visible' )
    xid.close!
  end

  test "Position with Window in the way" do
    # Open Terminal with position 40x100
    xid = open_window_at(40,100)
    sleep 0.5
    assert_equal( @app.Unity2dPanel()['x_absolute'], '-66', \
                  'Launcher visible when window in the way, should be hidden' )
    xid.close!
  end

  test "Move window positioning to check launcher action" do
    # Open Terminal with position 100x100
    xid = open_window_at(100,100)
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher hiding when window not in the way, should be visible' )
    xid.move(65,100)
    sleep 0.5
    assert_equal( @app.Unity2dPanel()['x_absolute'], '-66', \
                  'Launcher visible when window in the way, should be hidden' )
    xid.move(66,100)
    sleep 0.5
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher hiding when window not in the way, should be visible' )
    xid.close!
  end
  
  test "Reveal hidden Launcher with mouse" do
    xid = open_window_at(10,100)
    #Ensure mouse out of the way
    XDo::Mouse.move(100,200,10)
    assert_equal( @app.Unity2dPanel()['x_absolute'], '-66', \
                  'Launcher visible with window in the way, should be hidden' )
    XDo::Mouse.move(0,200)
    sleep 1
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher hiding when mouse at left edge of screen' )
    XDo::Mouse.move(65,200)
    sleep 2
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
                  'Launcher should still be visible as mouse over it' )
    XDo::Mouse.move(66,200)
    sleep 2
    assert_equal( @app.Unity2dPanel()['x_absolute'], '-66', \
                  'Launcher visible with window in the way and mouse moved out, should be hidden' )
    xid.close!
  end
end

