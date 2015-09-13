//
//  test_error_stop.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_error_stop_hpp
#define test_error_stop_hpp

class ErrorStopTestCtl : public CommonTestControl {
  public:
    ErrorStopTestCtl();
    virtual void runTest();
    virtual void initialize(SimModelPtr, SimModulePtr);
};

class TestErrorStop : public CommonTestModule {
    friend ErrorStopTestCtl;
    POutBool *clockError;
    POutBool *errorStopSignal;
  public:
    TestErrorStop(SimModelPtr, const std::string&, SimModulePtr);
};
#endif /* test_error_stop_hpp */
