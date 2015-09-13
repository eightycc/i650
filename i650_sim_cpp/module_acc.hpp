//
//  module_acc.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/28/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_acc_hpp
#define module_acc_hpp

class ModuleAccumulator : public SimModule {
    PortInInt *digitNumber;
    PortInInt *earlyDigitNumber;
    PortInBool *upperWord;
    PortInBiq *adder;
    PortInBool *readEarly;
    PortInBool *readAdder;
    PortInBool *zeroAcc;
    PortInBool *zeroShiftCtr;
    PortInBool *accReset;
    POutBiqReg *ontimeOut;
    POutBiqReg *earlyOut;
    POutBiq *pedOut;

    std::vector<BiqWord> accWords; //0:lower(even), 1:upper(odd)
    int  ontimeOdd;
    int  ontimeIdx;
    int  earlyOdd;
    int  earlyIdx;
    
  public:
    ModuleAccumulator(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual void reset();
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void cycleComboB();
    virtual void cycleSeqD();
    
    const BiqWord& readLower() const {return accWords[0];}
    void writeLower(const BiqWord& v);
    const BiqWord& readUpper() const {return accWords[1];}
    void writeUpper(const BiqWord& v);
    virtual void print(std::ostream& s, int level = 0) const;
};
#endif /* module_acc_hpp */
