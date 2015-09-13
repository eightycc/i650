//
//  test_addr_reg.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/22/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_addr_reg_hpp
#define test_addr_reg_hpp

class TestAddrReg;

class AddrRegTestControl : public CommonTestControl {
  public:
    AddrRegTestControl();
    virtual void runTest();
    virtual void initialize(SimModelPtr, SimModulePtr);
};

class TestAddrReg : public CommonTestModule {
    friend AddrRegTestControl;
    PortInBool  *dynamicHit;
    POutBool *restartA;
    POutBool *tluBandChange;
    POutBool *readInPgmStep;
public:
    TestAddrReg(SimModelPtr m, const std::string& nm, SimModulePtr p);
};

#endif /* test_addr_reg_hpp */
