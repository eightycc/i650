//
//  test_adder.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/21/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_adder_hpp
#define test_adder_hpp

class TestAdderCtl;

class AdderTestCtl : public CommonTestControl {
public:
    AdderTestCtl();
    virtual void runTest();
    virtual void initialize(SimModelPtr model, SimModulePtr top);
};

class TestAdderCtl : public CommonTestModule {
    friend AdderTestCtl;
    // adder mux a controls
    POutBool* accTrueAdd;
    POutBool* accCompAdd;
    POutBool* leftShiftOn;
    POutBool* pgmStepAdd;
    POutBool* shiftCount;
    POutBool* selStoreAdd;
    // adder mux b controls
    POutBool* distTrueAdd;
    POutBool* distCompAdd;
    POutBool* upperLower;
    POutBool* distOntimeAdd;
    POutBool* distBlank;
    POutBool* earlyDistZero;
    // adder controls
    POutBool* tluOn;
    //PortOutBool* leftShiftOn;
    POutBool* carryBlank;
    POutBool* carryInsert;
    POutBool* zeroInsert;
    POutBool* ovflwStopSw;
    POutBool* ovflwSenseSw;
    POutBool* shiftOvflw;
    POutBool* multDivOff;
    POutBool* quotDigitOn;
    //PortOutBool* distTrueAdd;
    //PortOutBool* accTrueAdd;
    // acc controls
    POutBool* readEarly;
    POutBool* readAdder;
    POutBool* zeroAcc;
    POutBool* zeroShift;
    // gs controls
    POutBool* readDist;
    POutBool* readBufStore;
public:
    TestAdderCtl(SimModelPtr m, const std::string& nm, SimModulePtr p);
    
};

#endif /* test_adder_hpp */
