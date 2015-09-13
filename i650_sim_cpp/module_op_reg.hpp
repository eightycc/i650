//
//  module_op_reg.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_op_reg_hpp
#define module_op_reg_hpp

class ModuleOpReg : public SimModule {
    PortInInt *digitNum;
    PortInBiq *progRegPed;
    PortInBool *restartA;
    PortInBool *restartB;
    PortInBool *dAlt;
    PortInBool *iAlt;
    PortInBool *tluBandChange;
    PortInBool *progReset;
    
    POutBiqReg *opregT;
    POutBiqReg *opregU;
    POutBool *readInAddrReg;
    
    BiqDigit tens;
    BiqDigit units;
    
  public:
    ModuleOpReg(SimModelPtr, const std::string&, SimObjPtr);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboB();
    virtual void cycleSeqB();
    virtual void cycleClockB();

    const BiqWord& readOpReg() const;  // opcode in d9, d10
    void writeOpReg(const BiqWord& v);
    // TODO: print method
    //virtual void print(std::ostream& s, int level = 0) const;
};

#endif /* module_op_reg_hpp */
