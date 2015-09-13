//
//  module_adder_in_a.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/8/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleAdderInA::ModuleAdderInA(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    accEarly      = new PortInBiq (m, "acc_early", this);
    accOntime     = new PortInBiq (m, "acc_ontime", this);
    pgmStepEarly  = new PortInBiq (m, "ps_early", this);
    addrUnits     = new PortInBiq (m, "addr_u", this);
    selectedStore = new PortInBiq (m, "sel_store", this);
    accTrueAdd    = new PortInBool(m, "acc_true_add", this);
    accCompAdd    = new PortInBool(m, "acc_comp_add", this);
    leftShift     = new PortInBool(m, "left_shift", this);
    pgmStepAdd    = new PortInBool(m, "pgm_step_add", this);
    shiftCount    = new PortInBool(m, "shift_count", this);
    selStoreAdd   = new PortInBool(m, "sel_store_add", this);
    entryA        = new POutBiq   (m, "entry_a", this, biqZero);
}

void ModuleAdderInA::cycleComboB() {
    *entryA =   (*accTrueAdd) ? *accEarly
              : (*accCompAdd) ? ((BiqDigit)*accEarly).ninesComplement()
              : (*leftShift)  ? *accOntime
              : (*pgmStepAdd) ? *pgmStepEarly
              : (*shiftCount) ? ((BiqDigit)*addrUnits).ninesComplement()
              : (*selStoreAdd)? *selectedStore
              : biqBlank;
}

vector<SimDispatchable> ModuleAdderInA::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    return dispatchables;
}
