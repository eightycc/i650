//
//  module_gen_store.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleGenStore::ModuleGenStore(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
  , memory(2000)
{
    digitNumber   = new PortInInt (m, "digit_num", this);
    wordNumber    = new PortInInt (m, "word_num", this);
    sectorNumber  = new PortInInt (m, "sector_num", this);
    earlyDigitNumber
                  = new PortInInt (m, "e_digit_num", this);
    addrThousands = new PortInBiq (m, "addr_th", this);
    addrHundreds  = new PortInBiq (m, "addr_h", this);
    addrTens      = new PortInBiq (m, "addr_t", this);
    distEarly     = new PortInBiq (m, "dist_early", this);
    bufStore      = new PortInBiq (m, "buf_store", this);
    readDist      = new PortInBool(m, "read_dist", this);
    readBufStore  = new PortInBool(m, "read_buf_store", this);
    genStoreOut   = new POutBiqReg(m, "gen_store_out", this, biqZero);
}
vector<SimDispatchable> ModuleGenStore::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    return dispatchables;
}
void ModuleGenStore::cycleSeqA() {
    BiqDigit thousands = *addrThousands;
    BiqDigit hundreds  = *addrHundreds;
    BiqDigit tens      = *addrTens;
    bool staticAddrValid = thousands.isValid() && hundreds.isValid() && tens.isValid();
    staticAddr = (staticAddrValid)?  thousands.decimal() * 1000
                                   + hundreds.decimal() * 100
                                   + ((tens.decimal() < 6)? 0 : 50)
                                  :  0;
    dynamicAddr = *sectorNumber * 10 + *wordNumber;
    ramAddr = staticAddr + dynamicAddr;
    //cout << "ramAddr: " << ramAddr
    //<< ", staticAddr: " << staticAddr
    //<< ", dynamicAddr: " << dynamicAddr
    //<< ", addr: " << (BiqDigit)*addrThousands << (BiqDigit)*addrHundreds << (BiqDigit)*addrTens << endl;
    if (ramAddr < 0 || ramAddr > 1999 || !staticAddrValid) {
        ramAddrValid = false;
        return;
    } else {
        ramAddrValid = true;
    }
    *genStoreOut = memory[ramAddr][*digitNumber].biq();
}
void ModuleGenStore::cycleClockA() {
    genStoreOut->cycle();
}
void ModuleGenStore::cycleSeqD() {
    if (!ramAddrValid) {
        return;
    }
    if (*readBufStore || *readDist) {
        int earlyAddr  = ramAddr;
        int earlyDigit = *earlyDigitNumber;
        if (earlyDigit == 11) {
            earlyAddr = (earlyAddr == 0)? 1999 : earlyAddr - 1;
        }
        memory[earlyAddr][earlyDigit] = (*readDist)? *distEarly : *bufStore;
    }
}
bool ModuleGenStore::loadDrum(const string& fn) {
    ifstream fileIn;
    fileIn.open(fn, ios_base::binary);
    if (!fileIn.good()) {
        cout << "*** Unable to open drum load file: " << fn << " ***" << endl;
        return false;
    }
    for (auto addr = 0; addr < 2000; addr++) {
        for (auto digit = ddx; digit <= dd10; digit++) {
            memory[addr][digit] = fileIn.get();
            if (!fileIn.good()) {
                cout << "*** Drum load file too small: " << fn << " ***" << endl;
                return false;
            }
        }
    }
    fileIn.close();
    return true;
}
bool ModuleGenStore::dumpDrum(const string& fn) {
    ofstream fileOut;
    fileOut.open(fn, ios_base::binary);
    if (!fileOut.good()) {
        cout << "*** Unable to open drum dump file: " << fn << " ***" << endl;
        return false;
    }
    for (auto addr = 0; addr < 2000; addr++) {
        for (auto digit = ddx; digit <= dd10; digit++) {
            fileOut.put(memory[addr][digit].biq());
            if (!fileOut.good()) {
                cout << "*** Failed to write drum dump file: " << fn << " ***" << endl;
                return false;
            }
        }
    }
    fileOut.close();
    return true;
}
void ModuleGenStore::reset() {
    for (auto addr = 0; addr < 2000; addr++) {
        memory[addr].blank();
    }
    staticAddr = 0;
    dynamicAddr = 0;
    ramAddrValid = false;
    ramAddr = 0;
}

void ModuleGenStore::print(ostream& s, int lvl) const {
    int indent = lvl + 1;
    SimModule::print(s, lvl);
    for (int addr = 0; addr < 2000; addr++) {
        if (0 == addr % 10) {
            for (int idt = indent; idt > 0; idt--)
                s << "  ";
            cout << setw(4) << setfill('0') << setbase(10)
                 << addr << ":";
        }
        //s << " " << memory[addr];
        s << " ";
        memory[addr].printReversed(s);
        if (9 == addr % 10) {
            s << endl;
        }
    }
}
