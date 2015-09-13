//
//  module_adder_in_b.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/15/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_adder_in_b_hpp
#define module_adder_in_b_hpp

class ModuleAdderInB : public SimModule {
    PortInBiq  *distEarly;
    PortInBiq  *distOntime;
    PortInBiq  *specialDigit;
    PortInBool *distOntimeAdd;
    PortInBool *distTrueAdd;
    PortInBool *distCompAdd;
    PortInBool *upperLowerSelect; // true during selected word time
    PortInBool *distBlank;
    PortInBool *earlyDistZero;
    POutBiq *entryB;
    
  public:
    ModuleAdderInB(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboB();
};

#endif /* module_adder_in_b_hpp */
