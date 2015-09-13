//
//  module_addr_reg.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/7/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleAddrReg::ModuleAddrReg(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    digitNumber   = new PortInInt  (m, "digit_num", this);
    wordNumber    = new PortInInt  (m, "word_num", this);
    sectorNumber  = new PortInInt  (m, "sector_num", this);
    errorReset    = new PortInBool (m, "error_reset", this);
    restartA      = new PortInBool (m, "restart_a", this);
    set8000       = new PortInBool (m, "set_8000", this);
    reset0000     = new PortInBool (m, "reset_0000", this);
    tluBandChange = new PortInBool (m, "tlu_band_chg", this);
    readIn        = new PortInBool (m, "read_in_ar", this);
    psPed         = new PortInBiq  (m, "ps_ped", this);
    transferKey   = new PortInBool (m, "xfer_key", this);
    ocAddrTh      = new PortInBiq  (m, "oc_addr_th", this);
    ocAddrH       = new PortInBiq  (m, "oc_addr_h", this);
    ocAddrT       = new PortInBiq  (m, "oc_addr_t", this);
    ocAddrU       = new PortInBiq  (m, "oc_addr_u", this);
    addrTh        = new POutBiqReg (m, "addr_th", this, biqZero);
    addrH         = new POutBiqReg (m, "addr_h", this, biqZero);
    addrT         = new POutBiqReg (m, "addr_t", this, biqZero);
    addrU         = new POutBiqReg (m, "addr_u", this, biqZero);
    addrHit       = new POutBool(m, "addr_hit", this, false);
    addrNo800x    = new POutBool(m, "addr_no_800x", this, true);
    addr8000      = new POutBool(m, "addr_8000", this, false);
    addr8001      = new POutBool(m, "addr_8001", this, false);
    addr8002      = new POutBool(m, "addr_8002", this, false);
    addr8003      = new POutBool(m, "addr_8003", this, false);
    addr8002_3    = new POutBool(m, "addr_8002_3", this, false);
    addrInvalid   = new POutBool(m, "addr_invalid", this, false);
    
    thousands = biqZero;
    hundreds  = biqZero;
    tens      = biqZero;
    units     = biqZero;
}

void ModuleAddrReg::cycleComboA() {
    bool addrInvalidP = false;
    int addr = 0;
    if (!thousands.isValid() || !hundreds.isValid() || !tens.isValid() || !units.isValid()) {
        addrInvalidP = true;
    } else {
        addr = thousands.decimal() * 1000 + hundreds.decimal() * 100 + tens.decimal() * 10 + units.decimal();
        if ((addr > 1999 && addr < 8000) || addr > 8003) {
            addrInvalidP = true;
        }
    }
    if (*errorReset || *readIn) {
        *addrInvalid = false;
    } else if (addrInvalidP) {
        *addrInvalid = true;
    }
    if (!addrInvalidP) {
        *addrNo800x = addr < 2000;
        *addr8000   = addr == 8000;
        *addr8001   = addr == 8001;
        *addr8002   = addr == 8002;
        *addr8003   = addr == 8003;
        *addr8002_3 = *addr8002 || *addr8003;
    }
    bool addrHitP = false;
    if (!addrInvalidP) {
        int nextDrumAddr = (*sectorNumber * 10 + *wordNumber + 1) % 50;
        if (nextDrumAddr == addr % 50) {
            addrHitP = true;
        }
    }
    if (*digitNumber == dd9) {
        *addrHit = addrHitP;
    } else if (*digitNumber == ddx) {
        *addrHit = false;
    }
}

void ModuleAddrReg::cycleSeqB() {
    bool resetP = (dd1 && *tluBandChange) || *restartA || *reset0000;
    if (*transferKey) {
        thousands = *ocAddrTh;
        hundreds = *ocAddrH;
        tens = *ocAddrT;
        units = *ocAddrU;
    } else if (*set8000) {
        thousands = biq8;
        hundreds = biqZero;
        tens = biqZero;
        units = biqZero;
    } else if (resetP) {
        thousands = biqZero;
        hundreds = biqZero;
        tens = biqZero;
        units = biqZero;
    } else if (*readIn) {
        switch (*digitNumber) {
            case dd4: case dd8: thousands = *psPed; break;
            case dd3: case dd7: hundreds = *psPed; break;
            case dd2: case dd6: tens = *psPed; break;
            case dd1: case dd5: units = *psPed; break;
            default: break;
        }
    }
    *addrTh = thousands;
    *addrH = hundreds;
    *addrT = tens;
    *addrU = units;
}

void ModuleAddrReg::cycleClockB() {
    addrTh->cycle();
    addrH->cycle();
    addrT->cycle();
    addrU->cycle();
}

vector<SimDispatchable> ModuleAddrReg::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aCombo, &SimModule::cycleComboA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bSeq, &SimModule::cycleSeqB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bClock, &SimModule::cycleClockB));
    return dispatchables;
}

const BiqWord& ModuleAddrReg::readAddrReg() const {
    addrRegDigits[dd8] = thousands;
    addrRegDigits[dd7] = hundreds;
    addrRegDigits[dd6] = tens;
    addrRegDigits[dd5] = units;
    return addrRegDigits;
}
void ModuleAddrReg::writeAddrReg(const BiqWord& v) {
    thousands = v[dd8];
    hundreds  = v[dd7];
    tens      = v[dd6];
    units     = v[dd5];
}

void ModuleAddrReg::reset() {
    thousands = biqZero;
    hundreds  = biqZero;
    tens      = biqZero;
    units     = biqZero;
}