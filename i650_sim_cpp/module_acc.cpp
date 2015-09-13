//
//  module_acc.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/28/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleAccumulator::ModuleAccumulator(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
  , accWords(2)
{
    digitNumber  = new PortInInt(m, "digit_num", this);
    earlyDigitNumber
                 = new PortInInt (m, "e_digit_num", this);
    upperWord    = new PortInBool(m, "upper_word", this);
    adder        = new PortInBiq (m, "adder", this);
    readEarly    = new PortInBool(m, "read_early", this);
    readAdder    = new PortInBool(m, "read_adder", this);
    zeroAcc      = new PortInBool(m, "zero_acc", this);
    zeroShiftCtr = new PortInBool(m, "zero_shift", this);
    accReset     = new PortInBool(m, "acc_reset", this);
    ontimeOut    = new POutBiqReg(m, "ontime_out", this, biqBlank, true);
    earlyOut     = new POutBiqReg(m, "early_out", this, biqBlank, true);
    pedOut       = new POutBiq(m, "ped_out", this, biqBlank, false);
}

vector<SimDispatchable> ModuleAccumulator::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    return dispatchables;
}

void ModuleAccumulator::cycleSeqA() {
    ontimeOdd = (*upperWord)? 1 : 0;
    ontimeIdx = *digitNumber;
    earlyOdd  = (ontimeIdx == 0)? (*upperWord)? 0 : 1
                                : ontimeOdd;
    earlyIdx  = *earlyDigitNumber;
    bool zeroP =   *accReset
                || ((ontimeIdx == dd0 || ontimeIdx == ddx) && ontimeOdd);
    *ontimeOut = (zeroP)? biqZero : *earlyOut;
    zeroP = ((!ontimeOdd && (ontimeIdx == dd10)) || (ontimeIdx == ddx))
            || *zeroAcc
            || (*zeroShiftCtr && !ontimeOdd && (ontimeIdx == dd1 || ontimeIdx == dd2));
    BiqDigit ramOut = accWords[earlyOdd][earlyIdx];
    *earlyOut = (zeroP)? biqZero : ramOut;
}

void ModuleAccumulator::cycleClockA() {
    ontimeOut->cycle();
    earlyOut->cycle();
}

void ModuleAccumulator::cycleComboB() {
    *pedOut = (*readEarly)? *earlyOut
            : (*readAdder)? (BiqDigit)*adder
            :               *ontimeOut;
}

void ModuleAccumulator::cycleSeqD() {
    accWords[ontimeOdd][ontimeIdx] = *pedOut;
}

void ModuleAccumulator::reset() {
    accWords[0].blank();
    accWords[1].blank();
}
void ModuleAccumulator::writeLower(const BiqWord& v) {
    accWords[0] = v;
    *earlyOut = accWords[earlyOdd][earlyIdx];
    earlyOut->cycle();
}
void ModuleAccumulator::writeUpper(const BiqWord& v) {
    accWords[1] = v;
    *earlyOut = accWords[earlyOdd][earlyIdx];
    earlyOut->cycle();
}
void ModuleAccumulator::print(ostream& s, int level) const {
    int indent = level + 1;
    SimModule::print(s, level);
    for (int idt = indent; idt > 0; idt--)
        s << "  ";
    s << "lower_acc: " << accWords[0]
    << " , upper_acc: " << accWords[1]
    << endl;
}

