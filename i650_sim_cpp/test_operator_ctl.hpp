//
//  test_operator_ctl.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/20/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_operator_ctl_hpp
#define test_operator_ctl_hpp

class OperatorTestCtl : public CommonTestControl {
    void testSwitch(ConsoleCommand, std::vector<int>);
    void testAddrSwitches(std::vector<BiqWord>);
    void testDataSwitches(std::vector<BiqWord>);
    void testButton(ConsoleCommand);
  public:
    OperatorTestCtl();
    virtual void runTest();
    virtual void initialize(SimModelPtr model, SimModulePtr top);
};

class TestOperatorCtl : public CommonTestModule {
    friend OperatorTestCtl;
    POutBool* restartReset;
  public:
    TestOperatorCtl(SimModelPtr m, const std::string& nm, SimModulePtr p);
    
};

#endif /* test_operator_ctl_hpp */
