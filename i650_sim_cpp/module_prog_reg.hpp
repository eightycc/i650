//
//  module_prog_reg.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_prog_reg_hpp
#define module_prog_reg_hpp

class ModuleProgReg : public SimModule {
    PortInInt *digitNumber;
    PortInInt *eDigitNumber;
    PortInBiq *adder;
    PortInBiq *selectedStore;
    PortInBool *progReset;
    PortInBool *rips;
    PortInBool *progAdd;
    
    POutBiqReg *ontimeOut;
    POutBiqReg *earlyOut;
    POutBiq *pedOut;
    POutBoolReg *progRestart;
    FFBoolReg *readIn;
    
    BiqWord digitRam;
    BiqDigit prevDigit;
    
  public:
    ModuleProgReg(SimModelPtr, const std::string&, SimObjPtr);
    virtual void reset();
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void cycleComboB();
    virtual void cycleSeqD();

    virtual void print(std::ostream& s, int level) const;
    const BiqWord& read() const {return digitRam;}
    void write(const BiqWord& v);
};

#endif /* module_prog_reg_hpp */
