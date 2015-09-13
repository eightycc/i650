//
//  module_adder.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/15/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_adder_hpp
#define module_adder_hpp

class ModuleAdder : public SimModule {
    PortInInt  *digitNum;
    PortInBool *upperWord;
    PortInBiq  *entryA;
    PortInBiq  *entryB;
    PortInBool *tluOn;
    PortInBool *leftShiftOn;
    PortInBool *carryBlank;
    PortInBool *carryInsert;
    PortInBool *zeroInsert;
    PortInBool *ovflwStopSw;
    PortInBool *ovflwSenseSw;
    PortInBool *shiftOvflw;
    PortInBool *multDivOff;
    PortInBool *quotDigitOn;
    PortInBool *distTrueAdd;
    PortInBool *accTrueAdd;
    PortInBool *errorReset;
    
    POutBiqReg  *sumOut;
    POutBoolReg *carryOut;
    POutBoolReg *ovflwStop;
    POutBoolReg *ovflwSense;
    
    FFBiq  *sum;
    FFBool *carry;
    FFBool *carryHold;
    FFBoolReg *adderReset;
  public:
    ModuleAdder(SimModelPtr m, const std::string& n, SimObjPtr p);
    //virtual void reset();
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void cycleSeqB();
    virtual void cycleClockB();
    virtual void cycleSeqC();
    virtual void cycleClockC();
    BiqDigit readSum() const;
    void writeSum(BiqDigit v);
    bool readCarry() const;
    void writeCarry(bool v);
    virtual void print(std::ostream& s, int level = 0) const;
};

#endif /* module_adder_hpp */
