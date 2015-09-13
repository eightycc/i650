//
//  module_dist.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleDistributor::ModuleDistributor(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    digitNumber = new PortInInt(m, "digit_num", this);
    eDigitNumber = new PortInInt(m, "e_digit_num", this);
    selectedStore = new PortInBiq(m, "store_select", this);
    accOntime = new PortInBiq(m, "acc_ontime", this);
    accReadIn = new PortInBool(m, "acc_readin", this);
    startAccReadIn = new PortInBool(m, "acc_ri_start", this);
    endAccReadIn = new PortInBool(m, "acc_ri_end", this);
    storeReadIn = new PortInBool(m, "ss_readin", this);
    accReset = new PortInBool(m, "acc_reset", this);
    
    ontimeOut = new POutBiqReg(m, "ontime_out", this, biqBlank);
    earlyOut = new POutBiqReg(m, "early_out", this, biqBlank);
    distBack = new POutBool(m, "dist_back", this, false);
    
    regenCtl = new FFBool(m, "regen_ctl", this, false);
    storeReadInCtl = new FFBool(m, "store_ri_ctl", this, false);
    accReadInDelay = new FFBool(m, "acc_ri_delay", this, false);
    accReadInCtl = new FFBool(m, "acc_ri_ctl", this, false);
}

vector<SimDispatchable> ModuleDistributor::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aCombo, &SimModule::cycleComboA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    return dispatchables;
}

void ModuleDistributor::cycleComboA() {
    if (*digitNumber == dd10) {
        *storeReadInCtl = false;
    } else if (*storeReadIn) {
        *storeReadInCtl = true;
    }
    
    if (*regenCtl && (*digitNumber == dd10 || *endAccReadIn)) {
        *regenCtl = false;
        *distBack = true;
    } else if (*storeReadIn || *startAccReadIn) {
        *regenCtl = true;
    } else {
        *distBack = false;
    }
    
    if (*accReadInDelay) {
        *accReadInCtl = true;
    } else if (!*accReadIn) {
        *accReadInCtl = false;
    }
    
    if (*accReadIn) {
        *accReadInDelay = true;
    } else {
        *accReadInDelay = false;
    }
}

void ModuleDistributor::cycleSeqA() {
    *ontimeOut = (*digitNumber == ddx)? biqZero
               : (*accReset)? ((*digitNumber == dd0)? biqPlus : biqZero)
               : prevDigit;
    prevDigit = (*regenCtl || *digitNumber == dd10)? biqBlank
               : distRam[*eDigitNumber];
    *earlyOut = prevDigit;
}

void ModuleDistributor::cycleClockA() {
    earlyOut->cycle();
    ontimeOut->cycle();
}

void ModuleDistributor::cycleSeqD() {
    distRam[*digitNumber] = (*storeReadInCtl)? (BiqDigit)*selectedStore
                          : (*accReadInCtl)? (BiqDigit)*accOntime
                          : (BiqDigit)*ontimeOut;
}

void ModuleDistributor::reset() {
    distRam.blank();
}
void ModuleDistributor::print(ostream& s, int level) const {
    int indent = level + 1;
    SimModule::print(s, level);
    for (int idt = indent; idt > 0; idt--)
        s << "  ";
    s << "distributor: " << distRam << endl;
}
void ModuleDistributor::write(const BiqWord& v) {
    distRam = v;
    prevDigit = distRam[*digitNumber];
}