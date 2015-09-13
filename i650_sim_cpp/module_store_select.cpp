//
//  module_store_select.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/18/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleStoreSelect::ModuleStoreSelect(SimModelPtr m, const string& n, SimObjPtr p)
: SimModule(m, n, p)
{
    digitNumber  = new PortInInt (m, "digit_num", this);
    accOntime    = new PortInBiq (m, "acc_ontime", this);
    distOntime   = new PortInBiq (m, "dist_ontime", this);
    genStoreOut  = new PortInBiq (m, "gen_store_out", this);
    dataSwitches = new PortInBiq (m, "data_switches", this);
    accPlus      = new PortInBool(m, "acc_plus", this);
    accMinus     = new PortInBool(m, "acc_minus", this);
    
    addrNo800x  = new PortInBool(m, "addr_no_800x", this);
    addr8000    = new PortInBool(m, "addr_8000", this);
    addr8001    = new PortInBool(m, "addr_8001", this);
    addr8002_3  = new PortInBool(m, "addr_8002_3", this);
    addrHot8000 = new PortInBool(m, "addr_hot_8000", this);
    
    selectedOut = new POutBiq(m, "select_out", this, biqBlank);
}

void ModuleStoreSelect::cycleComboB() {
    BiqDigit accSign =  (*accPlus)? BiqDigit::biqPlus
                      : (*accMinus)? BiqDigit::biqMinus
                      : biqBlank;
    BiqDigit accWithSign =  (*digitNumber >= dd1 && *digitNumber <= dd10)? *accOntime
                          : (*digitNumber == dd0)? accSign
                          : biqBlank;
    *selectedOut =  (*addrHot8000)? *dataSwitches
                  : (*addrNo800x)? *genStoreOut
                  : (*addr8000)? *dataSwitches
                  : (*addr8001)? *distOntime
                  : (*addr8002_3)? accWithSign
                  : biqBlank;
}

vector<SimDispatchable> ModuleStoreSelect::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bCombo, &SimModule::cycleComboB));
    return dispatchables;
}