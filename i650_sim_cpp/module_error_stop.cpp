//
//  module_error_stop.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleErrorStop::ModuleErrorStop(SimModelPtr m, const string& nm, SimObjPtr p)
  : SimModule(m, nm, p)
{
    digitNum = new PortInInt(m, "digit_num", this);
    lowerWord = new PortInBool(m, "lower_word", this);
    errSwSense = new PortInBool(m, "err_sw_sense", this);
    errorReset = new PortInBool(m, "err_reset", this);
    errorSenseReset = new PortInBool(m, "err_sense_reset", this);
    clockError = new PortInBool(m, "clock_error", this);
    errorStopSignal = new PortInBool(m, "err_stop_sig", this);
    ocBusy = new PortInBool(m, "oc_busy", this);
    
    errorSense = new POutBoolReg(m, "err_sense_on", this, false);
    errorStop = new POutBoolReg(m, "err_stop", this, false);
    errorSenseRestart = new POutBoolReg(m, "err_sense_restart", this, false);
    restartReset = new POutBoolReg(m, "restart_reset", this, false);
    turnOffRunLatch = new POutBoolReg(m, "turn_off_run", this, false);
    
    restartState = new FFIntReg(m, "reset_state", this, (int)RestartState::idle);
}

void ModuleErrorStop::cycleSeqB() {
    switch ((RestartState)(int)*restartState) {
        case RestartState::idle:
            if (*errorReset) {
                *errorStop = false;
            } else if (!*errorStop && (*clockError || *errorStopSignal)) {
                *errorStop = true;
                if (*errSwSense) {
                    *errorSense = true;
                    *restartState = (int)RestartState::sense1;
                } else {
                    *restartState = (int)RestartState::stop1;
                }
                timeOut = 0;
            }
            break;
        case RestartState::sense1:
            if (*ocBusy) {
                if (timeOut++ > 600) {
                    cout << "*** Error-stop restart timed out waiting for first oc busy to clear. ***" << endl;
                    *restartState = (int)RestartState::idle;
                }
            } else {
                *restartState = (int)RestartState::sense2;
            }
            break;
        case RestartState::sense2:
            if (!*lowerWord && *digitNum == ddx) {
                *restartReset = true;
                *turnOffRunLatch = true;
                timeOut = 0;
                *restartState = (int)RestartState::sense3;
            }
            break;
        case RestartState::sense3:
            *turnOffRunLatch = false;
            if (*ocBusy) {
                *restartReset = false;
                timeOut = 0;
                *restartState = (int)RestartState::sense4;
            } else {
                if (timeOut++ > 600) {
                    cout << "*** Error-stop restart timed out waiting for oc busy to come up. ***" << endl;
                    *restartState = (int)RestartState::idle;
                }
            }
            break;
        case RestartState::sense4:
            if (!*ocBusy && *lowerWord && *digitNum == dd10) {
                *errorStop = false;
                *errorSenseRestart = true;
                *restartState = (int)RestartState::sense5;
            } else {
                if (timeOut++ > 600) {
                    cout << "*** Error-stop restart timed out waiting for second oc busy to clear. ***" << endl;
                    *restartState = (int)RestartState::idle;
                }
            }
            break;
        case RestartState::sense5:
            *errorSenseRestart = false;
            *restartState = (int)RestartState::idle;
            break;
        case RestartState::stop1:
            *turnOffRunLatch = true;
            *restartState = (int)RestartState::stop2;
            break;
        case RestartState::stop2:
            *turnOffRunLatch = false;
            *restartState = (int)RestartState::idle;
            break;
    }
}

void ModuleErrorStop::cycleClockB() {
    errorSense->cycle();
    errorStop->cycle();
    errorSenseReset->cycle();
    errorSenseRestart->cycle();
    restartReset->cycle();
    turnOffRunLatch->cycle();
    restartState->cycle();
}

vector<SimDispatchable> ModuleErrorStop::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bSeq, &SimModule::cycleSeqB));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::bClock, &SimModule::cycleClockB));
    return dispatchables;
}