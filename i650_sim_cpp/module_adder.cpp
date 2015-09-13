//
//  module_adder.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/15/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

// Carry/no-carry: The adder maintains both carry and no-carry indicators.
// Together, they encode the adder carry-out. This implementation simplifies
// carry to a single indicator with carryBlank and carryInsert signals from
// arithmetic control to set/reset carryIn. Set/reset is forced when one or
// the other of carryBlank and carryInsert is asserted.
//
// carryBlank =   (wl & dx)
//              | (progAdd & d0)
//              | (wl & d0 & ~(accCompAdd | distCompAdd) & ~hcAdd5);
// carryInsert =  (d1 & selStoreAdd)
//              | (wl & d1 &  quotDigit & (accCompAdd | distCompAdd)
//              | (wl & d0 & ~quotDigit & (accCompAdd | distCompAdd);
//
// The carryInsert and carryBlank signals derive from cominatorial logic, so
// they remain asserted for a single word time and then fall.
//
// In the case of addition of like-signed numbers, carryBlank is on during
// dxl and d0l, forcing no carry input into the lowest-order addition. For
// complement addition (unlike signs), carryInsert is on during d0l. This
// is the default behavior, so no other action is necesary when using the
// adder as a 1 cycle delay; carryBlank gets asserted and zeroes are
// automatically supplied.
//
// Additional special cases exist for:
//   o blank at d0 when adding the program register (TLU only)
//   o half correction
//   o division
//   o adding from selected store mux (TLU?)
//
// The 650 retains carry in two pairs of complimentary latches: carry / no-
// carry, and carry test / no-carry test. The carry latches are reset by
// ap, except when adder reset is asserted. The carry test latches are
// always reset by ap. The carry latches are carry-in to the next add, so
// they need to retain their values while the acc crosses its unimplemented
// digits at dxu and d0u. The carry test latches supply carry outside the
// adder, so they are not smeared.
//
// When neither carryBlank nor carryInsert are asserted, then carryIn takes
// its value from the previous add, or retains its value if adderReset is
// asserted.
//
// adderResetOn  =   (wl & d10 & ~leftShift)
//                 | (wu &  dx &  leftShift);
// adderResetOff =   (wu &  d0 & ~leftShift)
//                 | (wu &  d1 &  leftShift)
//                 | (tlu);
//

ModuleAdder::ModuleAdder(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    digitNum     = new PortInInt (m, "digit_num", this);
    upperWord    = new PortInBool(m, "upper_word", this);
    entryA       = new PortInBiq (m, "entry_a", this);
    entryB       = new PortInBiq (m, "entry_b", this);
    tluOn        = new PortInBool(m, "tlu_on", this);
    leftShiftOn  = new PortInBool(m, "left_shift_on", this);
    carryBlank   = new PortInBool(m, "carry_blank", this);
    carryInsert  = new PortInBool(m, "carry_insert", this);
    zeroInsert   = new PortInBool(m, "zero_insert", this);
    ovflwStopSw  = new PortInBool(m, "ovflw_stop_sw", this);
    ovflwSenseSw = new PortInBool(m, "ovflw_sense_sw", this);
    shiftOvflw   = new PortInBool(m, "shift_ovflw", this);
    multDivOff   = new PortInBool(m, "mult_div_off", this);
    quotDigitOn  = new PortInBool(m, "quot_digit_on", this);
    distTrueAdd  = new PortInBool(m, "dist_true_add", this);
    accTrueAdd   = new PortInBool(m, "acc_true_add", this);
    errorReset   = new PortInBool(m, "error_reset", this);
    
    sumOut     = new POutBiqReg (m, "sum_out", this, biqBlank);
    carryOut   = new POutBoolReg(m, "carry_out", this, false);
    ovflwStop  = new POutBoolReg(m, "ovflw_stop", this, false);
    ovflwSense = new POutBoolReg(m, "ovflw_sense", this, false);
    
    sum        = new FFBiq (m, "sum", this, biqBlank);
    carry      = new FFBool(m, "carry", this, false);
    carryHold  = new FFBool(m, "carry_hold", this, false);
    adderReset = new FFBoolReg(m, "adder_reset", this, false);
}

void ModuleAdder::cycleSeqA() {
    bool resetCtlOnP  =   (!*upperWord && *digitNum == dd10 && !*leftShiftOn)
                       || ( *upperWord && *digitNum == ddx  &&  *leftShiftOn);
    bool resetCtlOffP =   ( *upperWord && *digitNum == dd0  && !*leftShiftOn)
                       || ( *upperWord && *digitNum == dd1  &&  *leftShiftOn)
                       || *tluOn;
    if (resetCtlOffP) {
        *adderReset = false;
    } else if (resetCtlOnP) {
        *adderReset = true;
    }
    *sumOut = *sum;
    *carryOut = *carry;
}
void ModuleAdder::cycleClockA() {
    adderReset->cycle();
    sumOut->cycle();
    carryOut->cycle();
}
void ModuleAdder::cycleSeqB() {
    *sum       =   (*zeroInsert)? biqZero
                 : (*adderReset)? *sum
                 : ((BiqDigit)*entryA).add(*entryB, *carryHold);
    *carry     =   (*carryBlank)? false
                 : (*carryInsert)? true
                 : ((BiqDigit)*entryA).carryFromAdd(*entryB, *carryHold);
    *carryHold =   (*carryBlank)? false
                 : (*carryInsert)? true
                 : (*adderReset)? *carryHold
                 : ((BiqDigit)*entryA).carryFromAdd(*entryB, *carryHold);
}
void ModuleAdder::cycleClockB() {
    sum->cycle();
    carry->cycle();
    carryHold->cycle();
}
void ModuleAdder::cycleSeqC() {
    bool ovflwP = (*carry && *upperWord && *digitNum == dd10 && *distTrueAdd
                   && *accTrueAdd && *multDivOff) || *shiftOvflw;
    bool stopP = (!*upperWord && *digitNum == dd1 && quotDigitOn)
                  || (ovflwP && *ovflwStopSw);
    bool senseP = ovflwP && *ovflwSenseSw;
    if (*errorReset) {
        *ovflwStop = false;
    } else if (stopP) {
        *ovflwStop = true;
    }
    *ovflwSense = senseP;
}
void ModuleAdder::cycleClockC() {
    ovflwStop->cycle();
    ovflwSense->cycle();
}

vector<SimDispatchable> ModuleAdder::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bSeq, &SimModule::cycleSeqB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bClock, &SimModule::cycleClockB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::cSeq, &SimModule::cycleSeqC));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::cClock, &SimModule::cycleClockC));
    return dispatchables;
}

void ModuleAdder::print(ostream& s, int level) const {
    int indent = level + 1;
    SimModule::print(s, level);
    for (int idt = indent; idt > 0; idt--)
        s << "  ";
    s << "sum_out: " << *sumOut
      << " ,carry_out: " << *carryOut
      << " ,sum: " << *sum
      << " ,carry: " << *carry
      << " ,ovflwStop: " << *ovflwStop
      << " ,ovflwSense: " << *ovflwSense
      << endl;
}
