//
//  module_addr_reg.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/7/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_addr_reg_hpp
#define module_addr_reg_hpp

class ModuleAddrReg : public SimModule {
    PortInInt *digitNumber;
    PortInInt *wordNumber;
    PortInInt *sectorNumber;
    PortInBool *errorReset;
    PortInBool *restartA;
    PortInBool *set8000;
    PortInBool *reset0000;
    PortInBool *tluBandChange;
    PortInBool *readIn;
    PortInBiq *psPed;
    PortInBool *transferKey;
    PortInBiq *ocAddrTh;
    PortInBiq *ocAddrH;
    PortInBiq *ocAddrT;
    PortInBiq *ocAddrU;
    POutBiqReg *addrTh;
    POutBiqReg *addrH;
    POutBiqReg *addrT;
    POutBiqReg *addrU;
    POutBool *addrHit;
    POutBool *addrNo800x;
    POutBool *addr8000;
    POutBool *addr8001;
    POutBool *addr8002;
    POutBool *addr8003;
    POutBool *addr8002_3;
    POutBool *addrInvalid;
    
    BiqDigit thousands; // todo: make these and addrRegDigits one
    BiqDigit hundreds;
    BiqDigit tens;
    BiqDigit units;
    mutable BiqWord addrRegDigits;
    
  public:
    ModuleAddrReg(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual void reset();
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboA();
    virtual void cycleSeqB();
    virtual void cycleClockB();
    
    const BiqWord& readAddrReg() const;
    void writeAddrReg(const BiqWord& v);
    // TODO: print method
    //virtual void print(std::ostream& s, int level = 0) const;
};

#endif /* module_addr_reg_hpp */
