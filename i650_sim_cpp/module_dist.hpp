//
//  module_dist.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_dist_hpp
#define module_dist_hpp

class ModuleDistributor : public SimModule {
    PortInInt   *digitNumber;
    PortInInt   *eDigitNumber;
    PortInBiq   *selectedStore;
    PortInBiq   *accOntime;
    PortInBool  *accReadIn;
    PortInBool  *startAccReadIn;
    PortInBool  *endAccReadIn;
    PortInBool  *storeReadIn;
    PortInBool  *accReset;
    
    POutBiqReg  *ontimeOut;
    POutBiqReg  *earlyOut;
    POutBool *distBack;
    
    BiqWord distRam;
    BiqDigit prevDigit;
    
    FFBool *regenCtl;
    FFBool *storeReadInCtl;
    FFBool *accReadInDelay;
    FFBool *accReadInCtl;
    
  public:
    ModuleDistributor(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual void reset();
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboA();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    //virtual void cycleComboB();
    virtual void cycleSeqD();
    
    virtual void print(std::ostream& s, int level) const;
    const BiqWord& read() const {return distRam;}
    void write(const BiqWord& v);
};

#endif /* module_dist_hpp */
