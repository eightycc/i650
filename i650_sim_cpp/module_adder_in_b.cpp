//
//  module_adder_in_b.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/15/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleAdderInB::ModuleAdderInB(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    distEarly        = new PortInBiq (m, "dist_early", this);
    distOntime       = new PortInBiq (m, "dist_ontime", this);
    specialDigit     = new PortInBiq (m, "special_digit", this);
    distOntimeAdd    = new PortInBool(m, "dist_ontime_add", this);
    distTrueAdd      = new PortInBool(m, "dist_true_add", this);
    distCompAdd      = new PortInBool(m, "dist_comp_add", this);
    upperLowerSelect = new PortInBool(m, "upper_lower", this);
    distBlank        = new PortInBool(m, "dist_blank", this);
    earlyDistZero    = new PortInBool(m, "early_dist_zero", this);
    entryB           = new POutBiq   (m, "entry_b", this, biqZero);

}

void ModuleAdderInB::cycleComboB() {
    bool distTrueAddP = *distTrueAdd | *upperLowerSelect | *distBlank;
    bool distCompAddP = *distCompAdd | *upperLowerSelect | *distBlank;
    *entryB =   (*earlyDistZero)? biqZero
              : (distTrueAddP)  ? *distEarly
              : (distCompAddP)  ? ((BiqDigit)*distEarly).ninesComplement()
              : (*distOntimeAdd)? *distOntime
              : *specialDigit;
}

vector<SimDispatchable> ModuleAdderInB::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    return dispatchables;
}
