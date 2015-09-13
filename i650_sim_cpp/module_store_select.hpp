//
//  module_store_select.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_store_select_hpp
#define module_store_select_hpp

class ModuleStoreSelect : public SimModule {
    PortInInt  *digitNumber;
    PortInBiq  *accOntime;
    PortInBiq  *distOntime;
    PortInBiq  *genStoreOut;
    PortInBiq  *dataSwitches;
    PortInBool *accPlus;
    PortInBool *accMinus;
    
    PortInBool *addrNo800x;
    PortInBool *addr8000;
    PortInBool *addr8001;
    PortInBool *addr8002_3;
    PortInBool *addrHot8000;
    
    POutBiq *selectedOut;

  public:
    ModuleStoreSelect(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboB();
};

#endif /* module_store_select_hpp */
