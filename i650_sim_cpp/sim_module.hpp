//
//  sim_module.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_module_hpp
#define sim_module_hpp

class SimModule : public SimObject {
  public:
    SimModule(SimModelPtr m, const std::string& n, SimObjPtr p);
    
    virtual std::vector<SimDispatchable> getDispatchables() {
        return std::vector<SimDispatchable>();
    }
    virtual void cycleComboA() {}
    virtual void cycleSeqA()   {}
    virtual void cycleClockA() {}
    virtual void cycleComboB() {}
    virtual void cycleSeqB()   {}
    virtual void cycleClockB() {}
    virtual void cycleComboC() {}
    virtual void cycleSeqC()   {}
    virtual void cycleClockC() {}
    virtual void cycleComboD() {}
    virtual void cycleSeqD()   {}
    virtual void cycleClockD() {}
};

#endif /* sim_module_hpp */
