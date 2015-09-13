//
//  module_ctl_comm.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

// Control Commutator notes from 650 patent:
//
// Restart A is reset during D of DX when Restart B is set.
// Restart B is set at C and reset at A (WP) of DX
// I Alternation is reset during D of DX when Restart B is set, as is
// Alternation. Setting occurs when its opposite is turned off, so it
// also occurs sometime during D.
// Manual Start - Stop is "clocked" by the start switch.
// Run Latch is not clocked
//

ModuleCtlComm::ModuleCtlComm(SimModelPtr m, const std::string& nm, SimObjPtr p)
  : SimModule(m, nm, p)
{
    digitNum = new PortInInt(m, "digit_num", this);
    lowerWord = new PortInBool(m, "lower_word", this);
    pgmReset = new PortInBool(m, "pgm_reset", this);
    errorReset = new PortInBool(m, "err_reset", this);
    dispSwReadOut = new PortInBool(m, "disp_sw_readout", this);
    dispSwReadIn = new PortInBool(m, "disp_sw_readin", this);
    hcSwHalf = new PortInBool(m, "hc_sw_half", this);
    startKey = new PortInBool(m, "pgm_start", this);
    stopKey = new PortInBool(m, "pgm_stop", this);
    stopAddr = new PortInBool(m, "pgm_stop_addr", this);
    opcodeT = new PortInBiq(m, "opreg_t", this);
    opcodeU = new PortInBiq(m, "opreg_u", this);
    addrNo800x = new PortInBool(m, "addr_no_800x", this);
    
    pgmRestart = new PortInBool(m, "pgm_restart", this);
    arithRestart = new PortInBool(m, "arith_restart", this);
    decodeRestarts = new PortInBool(m, "decode_restarts", this);
    addrInvalid = new PortInBool(m, "addr_invalid", this);
    opStop = new PortInBool(m, "op_stop", this);
    restartA = new POutBoolReg(m, "restart_a", this, false);
    restartB = new POutBoolReg(m, "restart_b", this, false);
    
    runControl = new PortInBool(m, "run_control", this);
    useDforI = new PortInBool(m, "use_d_for_i", this);
    iAlt = new POutBoolReg(m, "i_alt", this, false);
    dAlt = new POutBoolReg(m, "d_alt", this, false);
    iControl = new POutBool(m, "i_control", this, false);
    dControl = new POutBool(m, "d_control", this, false);
    readPunchDCtl = new POutBool(m, "read_punch_d_ctl", this, false);
    
    errSenseRestart = new PortInBool(m, "err_sns_restart", this);
    errStop = new PortInBool(m, "err_stop", this);
    distBack = new PortInBool(m, "dist_back", this);
    startStopLatch = new FFBoolEdgeReg(m, "start_stop", this, false, true);
    runLatch = new FFBoolEdgeReg(m, "run_latch", this, false, true);
    
    manReadOutStore = new POutBoolReg(m, "man_ro_store", this, false);
    readInOperand = new FFBoolReg(m, "read_in_operand", this, false);
    opInterlock = new POutBoolReg(m, "op_interlock", this, false);
    singleInterlock = new POutBoolReg(m, "single_intlk", this, false);
    
    op20_24_d = new FFBool(m, "op20_24_d", this, false);
    op20_24_d_intlk = new FFBool(m, "op20_24_d_intlk", this, false);
    op20_start = new FFBool(m, "op20_start", this, false);
    op21_start = new FFBool(m, "op21_start", this, false);
    op22_start = new FFBool(m, "op22_start", this, false);
    op23_start = new FFBool(m, "op23_start", this, false);
    op20_stop = new FFBool(m, "op20_stop", this, false);
    op21_stop = new FFBool(m, "op21_stop", this, false);
    op22_stop = new FFBool(m, "op22_stop", this, false);
    op23_stop = new FFBool(m, "op23_stop", this, false);
    op24 = new FFBool(m, "op24", this, false);
}

vector<SimDispatchable> ModuleCtlComm::getDispatchables() {
    std::vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aCombo, &SimModule::cycleComboA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dClock, &SimModule::cycleClockD));
    return dispatchables;
}

void ModuleCtlComm::cycleComboA() {
    // I/D Alternation
    *iControl = *runControl && !*restartB && *iAlt;
    *dControl = *runControl && !*restartB && !*opInterlock && *dAlt;
    *readPunchDCtl = *runControl && !*restartB && *dAlt;
    
    // Decode Store Ops
    *op20_24_d = (BiqDigit)*opcodeT == biq2 && ((int)(BiqDigit)*opcodeU & BiqDigit::biq_q0) && *dControl;
    *op20_24_d_intlk = *op20_24_d && !*singleInterlock;
    *op20_start = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq0 && !*lowerWord && *digitNum == dd10;
    *op21_start = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq1 && *lowerWord && *digitNum == dd10;
    *op22_start = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq2 && *lowerWord && *digitNum == dd3;
    *op23_start = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq3 && *lowerWord && *digitNum == ddx;
    *op20_stop = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq0 && *digitNum == dd9;
    *op21_stop = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq1 && !*lowerWord && *digitNum == dd9;
    *op22_stop = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq2 && *digitNum == dd7;
    *op23_stop = *op20_24_d_intlk && (BiqDigit)*opcodeU == biq3 && *digitNum == dd3;
}

void ModuleCtlComm::cycleSeqA() {
    // ---------------
    // Restart control
    // ---------------
    bool allRestarts = *pgmRestart || *arithRestart || *decodeRestarts;
    
    bool restartAoffP = *pgmReset || (*restartB && *digitNum == ddx && *runControl);
    bool restartAonP = allRestarts && !*restartB && !*addrInvalid;
    
    if (restartAoffP) {
        *restartA = false;
    } else if (restartAonP) {
        *restartA = true;
    }
    
    if (*restartA && *digitNum == ddx) {
        *restartB = true;
    } else if (*digitNum == ddx) {
        *restartB = false;
    }
    
    // -----------
    // Run control
    // -----------
    // The "manual start-stop latch"  
    bool startP = *digitNum == ddx && !*lowerWord && !*errStop && *startKey;
    if (startP) {
        *startStopLatch = true;
    } else {
        *startStopLatch = false;
    }
    
}

void ModuleCtlComm::cycleClockA() {
    restartA->cycle();
    restartB->cycle();
    startStopLatch->cycle();
    iControl->cycle();
    dControl->cycle();
}

void ModuleCtlComm::cycleSeqD() {
    bool altReset = *pgmReset  || *restartA || *useDforI;
    bool alternate = *runControl && *restartB && *digitNum == ddx;
    
    // I/D alternation
    if (altReset) {
        // starting in iAlt state at restartA (or pgmReset)
        *iAlt = false;
        *dAlt = true;
    } else {
        if (alternate) {
            // flip at dx:d of restartB
            *iAlt = !*iAlt;
            *dAlt = !*dAlt;
        }
    }
    
    // Run control
    bool runLatchReset = ((*hcSwHalf || *stopKey) && restartA) || *stopAddr || *opStop || *errorReset;
    bool runLatchOn = *startStopLatch || *errSenseRestart;
    if (runLatchReset) {
        runLatch->reset();
    } else if (runLatchOn) {
        *runLatch = true;
    }
#if 0
    bool runLatchOn = start || *errSenseRestart;
    if (((*hcSwHalf || *stopKey) && restartA) || *stopAddr || *opStop) {
        runLatchOff = true;
    }
    
    
    bool started = false;
    if (startP && !*startFired) {
        started = true;
        *startFired = true;
    }
    if (*digitNum == ddx && !*lowerWord) {
        if (!*startFired && (!*errStop || *startKey)) {
            *startFired = true;
        }
    }
    bool runLatchOn =
    
    bool manReadOutStoreP = *dispSwReadOut && *runLatch;
    bool readInOperandP = dControlP && *addrNo800x
    && ((BiqDigit)*orT == biq1 || (BiqDigit)*orT == biq6);
#endif
}

void ModuleCtlComm::cycleClockD() {
    iAlt->cycle();
    dAlt->cycle();
    runLatch->cycle();
}
