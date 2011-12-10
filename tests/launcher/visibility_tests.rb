require '../run-tests.rb' unless $INIT_COMPLETED

context "Launcher Tests" do
  startup do
    puts "Startup"
  end
  
  shutdown do
    puts "Shutdown"
  end

  setup do
    # Execute the application 
    @sut = TDriver.sut(:Id => "sut_qt")    
    @app = @sut.run( :name => UNITY_2D_LAUNCHER, 
    				 :arguments => "-testability", 
    				 :sleeptime => 1 )
  end

  teardown do
    #@app.close        
    #Need to kill Launcher as it does not shutdown when politely asked
    system "pkill -nf unity-2d-launcher"
  end

  test "Start with Empty Desktop" do
    assert_equal( @app.Unity2dPanel()['x_absolute'], '0', \
    			  'Launcher hiding on empty desktop, should be visible' )
  end

end
