//
//  test_prog_reg.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_prog_reg_hpp
#define test_prog_reg_hpp

class TestProgReg;

class ProgRegTestControl : public CommonTestControl {
  public:
    ProgRegTestControl();
    virtual void runTest();
    virtual void initialize(SimModelPtr, SimModulePtr);
};

class TestProgReg : public CommonTestModule {
    friend ProgRegTestControl;
    POutBool *rips;
    POutBool *progAdd;
  public:
    TestProgReg(SimModelPtr, const std::string&, SimModulePtr);
};

#endif /* test_prog_reg_hpp */
