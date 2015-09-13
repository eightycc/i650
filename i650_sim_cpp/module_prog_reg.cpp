//
//  module_prog_reg.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleProgReg::ModuleProgReg(SimModelPtr m, const string& nm, SimObjPtr p)
  : SimModule(m, nm, p)
{
    digitNumber = new PortInInt(m, "digit_num", this);
    eDigitNumber = new PortInInt(m, "e_digit_num", this);
    adder = new PortInBiq(m, "adder", this);
    selectedStore = new PortInBiq(m, "store_select", this);
    progReset = new PortInBool(m, "prog_reset", this);
    rips = new PortInBool(m, "rips", this);
    progAdd = new PortInBool(m, "prog_add", this);
    
    ontimeOut = new POutBiqReg(m, "ontime_out", this, biqBlank);
    earlyOut = new POutBiqReg(m, "early_out", this, biqBlank);
    pedOut = new POutBiq(m, "ped_out", this, biqBlank);
    progRestart = new POutBoolReg(m, "prog_restart", this, false);
    readIn = new FFBoolReg(m, "read_in", this, false);
}
vector<SimDispatchable> ModuleProgReg::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    return dispatchables;
}
void ModuleProgReg::cycleSeqA() {
    *earlyOut = digitRam[*eDigitNumber];
    *ontimeOut = (*progReset)? biqZero : *earlyOut;
    if (*digitNumber == ddx && *readIn) {
        *readIn = false;
        *progRestart = true;
    } else {
        *progRestart = false;
        if (*rips) {
            *readIn = true;
        }
    }
}
void ModuleProgReg::cycleClockA() {
    earlyOut->cycle();
    ontimeOut->cycle();
    readIn->cycle();
    progRestart->cycle();
}
void ModuleProgReg::cycleComboB() {
    *pedOut = (*readIn)? *selectedStore
    : (*progAdd)? (BiqDigit)*adder
    : *ontimeOut;
}
void ModuleProgReg::cycleSeqD() {
    digitRam[*digitNumber] = *pedOut;
}
void ModuleProgReg::reset() {
    digitRam.blank();
}
void ModuleProgReg::print(ostream& s, int level) const {
    int indent = level + 1;
    SimModule::print(s, level);
    for (int idt = indent; idt > 0; idt--)
        s << "  ";
    s << "Program Step Register: " << digitRam << endl;
}
void ModuleProgReg::write(const BiqWord& v) {
    digitRam = v;
    prevDigit = digitRam[*digitNumber];
}