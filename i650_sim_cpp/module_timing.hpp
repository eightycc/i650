//
//  module_timing.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_timing_hpp
#define module_timing_hpp

class ModuleTiming : public SimModule {
    long long cycleNumber = 1;
    POutIntReg *digitNumber;
    POutIntReg *wordNumber;
    POutIntReg *sectorNumber;
    POutIntReg *earlyDigitNumber;
    POutBoolReg *upperWord;
    POutBoolReg *lowerWord;
    
  public:
    ModuleTiming(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void reset();
    virtual void cycleSeqD();
    virtual void cycleClockD();
    int getDigitNumber() const {return *digitNumber;}
    bool getUpperWord() const {return *upperWord;}
};

#endif /* module_timing_hpp */
