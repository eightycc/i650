//
//  module_top.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_top_hpp
#define module_top_hpp

class ModuleTop : public SimModule {
    SimModule *opCtlModule;
    SimModule *accModule;
    SimModule *timingModule;
    SimModule *genStoreModule;
    SimModule *addrRegModule;
    SimModule *adderInAModule;
    SimModule *adderInBModule;
    SimModule *adderModule;
    SimModule *storeSelModule;
    SimModule *distModule;
    SimModule *progRegModule;
    SimModule *errStopModule;
    SimModule *opRegModule;
    SimModule *ctlCommModule;
  public:
    ModuleTop(SimModelPtr m);
};

#endif /* module_top_hpp */
