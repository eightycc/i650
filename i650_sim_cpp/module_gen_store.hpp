//
//  module_gen_store.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_gen_store_hpp
#define module_gen_store_hpp

class ModuleGenStore : public SimModule {
    PortInInt *digitNumber;
    PortInInt *wordNumber;
    PortInInt *sectorNumber;
    PortInInt *earlyDigitNumber;
    PortInBiq *addrThousands;
    PortInBiq *addrHundreds;
    PortInBiq *addrTens;
    PortInBiq *distEarly;
    PortInBiq *bufStore;
    PortInBool *readDist;
    PortInBool *readBufStore;
    POutBiqReg *genStoreOut;
    
    std::vector<BiqWord> memory;
    int  staticAddr;
    int  dynamicAddr;
    bool ramAddrValid;
    int  ramAddr;
  public:
    ModuleGenStore(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void cycleSeqD();
    
    const BiqWord& readDrum(int addr) const {return memory[addr];}
    void writeDrum(int addr, const BiqWord& word) {memory[addr] = word;}
    bool loadDrum(const std::string& fn);
    bool dumpDrum(const std::string& fn);

    virtual void reset();
    virtual void print(std::ostream& s, int level = 0) const;
};
#endif /* module_gen_store_hpp */
