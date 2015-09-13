//
//  module_op_reg.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleOpReg::ModuleOpReg(SimModelPtr m, const string& nm, SimObjPtr p)
  : SimModule(m, nm, p)
{
    digitNum = new PortInInt(m, "digit_num", this);
    progRegPed = new PortInBiq(m, "prog_reg_ped", this);
    restartA = new PortInBool(m, "restart_a", this);
    restartB = new PortInBool(m, "restart_b", this);
    dAlt = new PortInBool(m, "d_alt", this);
    iAlt = new PortInBool(m, "i_alt", this);
    tluBandChange = new PortInBool(m, "tlu_band_chg", this);
    progReset = new PortInBool(m, "prog_reset", this);
    
    opregT = new POutBiqReg(m, "opreg_t", this, biqBlank);
    opregU = new POutBiqReg(m, "opreg_u", this, biqBlank);
    readInAddrReg = new POutBool(m, "ri_addr_reg", this, false);
}

void ModuleOpReg::cycleComboB() {
    *readInAddrReg = (*dAlt && *restartB && *digitNum >= dd5 && *digitNum < dd9)
                  || (*iAlt && *restartB && *digitNum >= dd1 && *digitNum < dd5)
                  || (*tluBandChange && *digitNum >= dd1 && *digitNum < dd5);
}

void ModuleOpReg::cycleSeqB() {
    bool opRegResetP = *progReset || *restartA || (*tluBandChange && *digitNum == dd0);
    if (opRegResetP) {
        *opregT = biqBlank;
        *opregU = biqBlank;
    } else if (readInAddrReg) {
        if (*digitNum == dd9) {
            *opregU = *progRegPed;
        } else if (*digitNum == dd10) {
            *opregT = *progRegPed;
        }
    }
}

void ModuleOpReg::cycleClockB() {
    opregT->cycle();
    opregU->cycle();
}

vector<SimDispatchable> ModuleOpReg::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bSeq, &SimModule::cycleSeqB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bClock, &SimModule::cycleClockB));
    return dispatchables;
}