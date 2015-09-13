//
//  adder_in_a.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/8/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_adder_in_a_hpp
#define module_adder_in_a_hpp

class ModuleAdderInA : public SimModule {
    PortInBiq *accEarly;
    PortInBiq *accOntime;
    PortInBiq *pgmStepEarly;
    PortInBiq *addrUnits;
    PortInBiq *selectedStore;
    PortInBool *accTrueAdd;
    PortInBool *accCompAdd;
    PortInBool *leftShift;
    PortInBool *pgmStepAdd;
    PortInBool *shiftCount;
    PortInBool *selStoreAdd;
    POutBiq *entryA;
    
  public:
    ModuleAdderInA(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboB();
};

#endif /* module_adder_in_a_hpp */
