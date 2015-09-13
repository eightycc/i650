//
//  test_ctl_comm.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 9/4/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_ctl_comm_hpp
#define test_ctl_comm_hpp

class TestAddrReg;

class CtlCommTestControl : public CommonTestControl {
public:
    CtlCommTestControl();
    virtual void runTest();
    virtual void initialize(SimModelPtr, SimModulePtr);
};

class TestCtlComm : public CommonTestModule {
    friend CtlCommTestControl;
    POutBool *pgmRestart;
    POutBool *arithRestart;
    POutBool *decodeRestarts;
    POutBool *useDforI;
public:
    TestCtlComm(SimModelPtr m, const std::string& nm, SimModulePtr p);
};


#endif /* test_ctl_comm_hpp */
