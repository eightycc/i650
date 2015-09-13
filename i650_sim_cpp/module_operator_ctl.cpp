//
//  module_operator_ctl.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/8/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleOperatorCtl::ModuleOperatorCtl(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    digitNumber      = new PortInInt  (m, "digit_num", this);
    wordNumber       = new PortInInt  (m, "word_num", this);
    upperWord        = new PortInBool (m, "upper_word", this);
    sectorNumber     = new PortInInt  (m, "sector_num", this);
    earlyDigitNumber = new PortInInt  (m, "e_digit_num", this);
    selectedStore    = new PortInBiq  (m, "sel_store", this);
    accOntime        = new PortInBiq  (m, "acc_ontime", this);
    pgmStepOntime    = new PortInBiq  (m, "pgm_step_ontime", this);
    restartReset     = new PortInBool (m, "restart_reset", this);
    errorReset       = new POutBoolReg(m, "error_reset", this, false);
    errorSenseReset  = new POutBoolReg(m, "err_sense_reset", this, false);
    programReset     = new POutBoolReg(m, "pgm_reset", this, false);
    accReset         = new POutBoolReg(m, "acc_reset", this, false);
    set8000          = new POutBoolReg(m, "set_8000", this, false);
    reset0000        = new POutBoolReg(m, "reset_0000", this, false);
    runControl       = new POutBool(m, "run_control", this, false);
    storageControl   = new POutBool(m, "store_control", this, false);
    halfCycleOrProgramStop
                     = new POutBoolReg(m, "hc_or_pgm_stop", this, false);
    programStart     = new POutBoolReg(m, "pgm_start", this, false);
    programStop      = new POutBoolReg(m, "pgm_stop", this, false);
    readInStorage    = new POutBool(m, "ri_store", this, false);
    readOutStorage   = new POutBool(m, "ro_store", this, false);
    addrSwitchTh     = new POutBiq (m, "addr_th", this, biq8);
    addrSwitchH      = new POutBiq (m, "addr_h", this, biqZero);
    addrSwitchT      = new POutBiq (m, "addr_t", this, biqZero);
    addrSwitchU      = new POutBiq (m, "addr_u", this, biqZero);
    dataSwitchesOut  = new POutBiqReg (m, "data_out", this, biqZero);
    pgmSwStop        = new POutBool(m, "pgm_sw_stop", this, false);
    pgmSwRun         = new POutBool(m, "pgm_sw_run", this, false);
    hcSwRun          = new POutBool(m, "hc_sw_run", this, false);
    hcSwHalf         = new POutBool(m, "hc_sw_half", this, false);
    ctlSwAddrStop    = new POutBool(m, "ctl_sw_stop", this, false);
    ctlSwRun         = new POutBool(m, "ctl_sw_run", this, false);
    ctlSwMan         = new POutBool(m, "ctl_sw_man", this, false);
    ovflwSwStop      = new POutBool(m, "ovflw_sw_stop", this, false);
    ovflwSwSense     = new POutBool(m, "ovflw_sw_sense", this, false);
    errSwStop        = new POutBool(m, "err_sw_stop", this, false);
    errSwSense       = new POutBool(m, "err_sw_sense", this, false);
    dispSwAccUp      = new POutBool(m, "disp_sw_acc_up", this, false);
    dispSwAccLow     = new POutBool(m, "disp_sw_acc_low", this, false);
    dispSwDist       = new POutBool(m, "disp_sw_dist", this, false);
    dispSwPgmReg     = new POutBool(m, "disp_sw_pr", this, false);
    dispSwReadIn     = new POutBool(m, "disp_sw_readin", this, false);
    dispSwReadOut    = new POutBool(m, "disp_sw_readout", this, false);
    transferKey      = new POutBool(m, "xfer_key", this, false);
    commandIn        = new PortInInt  (m, "command_in", this);
    commandOperand   = new PortInBiqWord
                                      (m, "command_op", this);
    commandSwitchSetting
                     = new PortInInt  (m, "command_sw", this);
    busy             = new POutBoolReg(m, "busy", this, true);
    
    currentStateDebug
                     = new POutIntReg (m, "current_state", this, 0);
    nextStateDebug   = new POutIntReg (m, "next_state", this, 0);
    currentCommandDebug
                     = new POutIntReg (m, "current_command", this, 0);
    nextCommandDebug = new POutIntReg (m, "next_command", this, 0);
}

void ModuleOperatorCtl::processCommands() {
    // Processing a command can complete immediately, as in the case of console
    // switch settings, or post a work flag for later processing.
    switch (currentCommand) {
        case ConsoleCommand::none:
            if (*restartReset) {
                doPgmReset = true;
                doAccReset = true;
                doErrReset = true;
                *busy = true;
            } else if (doPowerOnReset) {
                doPowerOnReset = false;
                doResetConsole = true;
                doErrReset = true;
                doErrSenseReset = true;
                doPgmReset = true;
                doAccReset = true;
                *busy = true;
            } else if (doResetConsole) {
                doResetConsole = false;
                resetControls();
                *busy = true;
            } else if (doPgmReset) {
                doPgmReset = false;
                nextState = ConsoleState::programReset1;
                *busy = true;
            } else if (doAccReset) {
                doAccReset = false;
                nextState = ConsoleState::accReset1;
                *busy = true;
            } else if (doErrReset) {
                doErrReset = false;
                nextState = ConsoleState::errorReset1;
                *busy = true;
            } else if (doErrSenseReset) {
                doErrSenseReset = false;
                nextState = ConsoleState::errorSenseReset1;
                *busy = true;
            } else {
                currentCommand = nextCommand;
                currentSwSetting = nextSwSetting;
                currentOperand = nextOperand;
                nextCommand = (ConsoleCommand)(int)*commandIn;
                nextSwSetting = *commandSwitchSetting;
                nextOperand = *commandOperand;
                *busy = false;
            }
            break;
        // Console switch settings
        case ConsoleCommand::setProgramSwitch:
            currentCommand = ConsoleCommand::none;
            programSwitch = (ProgramSwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setHalfCycleSwitch:
            currentCommand = ConsoleCommand::none;
            halfCycleSwitch = (HalfCycleSwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setControlSwitch:
            currentCommand = ConsoleCommand::none;
            controlSwitch = (ControlSwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setDisplaySwitch:
            currentCommand = ConsoleCommand::none;
            displaySwitch = (DisplaySwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setOverflowSwitch:
            currentCommand = ConsoleCommand::none;
            overflowSwitch = (OverflowSwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setErrorSwitch:
            currentCommand = ConsoleCommand::none;
            errorSwitch = (ErrorSwitch)currentSwSetting;
            *busy = true;
            break;
        case ConsoleCommand::setStorageEntrySwitches:
            currentCommand = ConsoleCommand::none;
            storageEntrySwitches = currentOperand;
            *busy = true;
            break;
        case ConsoleCommand::setAddressSelectSwitches:
            currentCommand = ConsoleCommand::none;
            addrSelectSwitches.plusZero();
            for (auto i = dd4; i >= dd1; i--) {
                addrSelectSwitches[i] = currentOperand[i];
            }
            *busy = true;
            break;
        // Console key presses
        case ConsoleCommand::pressTransferKey:
            currentCommand = ConsoleCommand::none;
            *busy = true;
            if (controlSwitch == ControlSwitch::manual) {
                nextState = ConsoleState::xferKey1;
            } else {
                cout << "*** Transfer key ignored, must be in manual. ***" << endl;
            }
            break;
        case ConsoleCommand::pressProgramStartKey:
            currentCommand = ConsoleCommand::none;
            *busy = true;
            nextState = ConsoleState::programStartKey1;
            break;
        case ConsoleCommand::pressProgramStopKey:
            // TODO
            break;
        case ConsoleCommand::pressProgramResetKey:
            currentCommand = ConsoleCommand::none;
            doPgmReset = true;
            *busy = true;
            break;
        case ConsoleCommand::pressComputerResetKey:
            currentCommand = ConsoleCommand::none;
            doPgmReset = true;
            doAccReset = true;
            doErrReset = true;
            *busy = true;
            break;
        case ConsoleCommand::pressAccResetKey:
            currentCommand = ConsoleCommand::none;
            doAccReset = true;
            *busy = true;
            break;
        case ConsoleCommand::pressErrorResetKey:
            currentCommand = ConsoleCommand::none;
            doErrReset = true;
            *busy = true;
            break;
        case ConsoleCommand::pressErrorSenseResetKey:
            currentCommand = ConsoleCommand::none;
            doErrSenseReset = true;
            *busy = true;
            break;
        case ConsoleCommand::resetConsole:
            currentCommand = ConsoleCommand::none;
            doResetConsole = true;
            *busy = true;
            break;
    }
    refreshSwitches();
}

void ModuleOperatorCtl::refreshSwitches() {
    // A good example of a case where logic runs infrequently; this routine
    // is called at the conclusion of all commands.
    *pgmSwStop = (programSwitch == ProgramSwitch::stop)? true : false;
    *pgmSwRun = (programSwitch == ProgramSwitch::run)? true : false;
    *hcSwRun = (halfCycleSwitch == HalfCycleSwitch::run)? true : false;
    *hcSwHalf = (halfCycleSwitch == HalfCycleSwitch::half)? true : false;
    *ctlSwAddrStop = (controlSwitch == ControlSwitch::addrStop)? true : false;
    *ctlSwRun = (controlSwitch == ControlSwitch::run)? true : false;
    *ctlSwMan = (controlSwitch == ControlSwitch::manual)? true : false;
    *ovflwSwStop = (overflowSwitch == OverflowSwitch::stop)? true : false;
    *ovflwSwSense = (overflowSwitch == OverflowSwitch::sense)? true : false;
    *errSwStop = (errorSwitch == ErrorSwitch::stop)? true : false;
    *errSwSense = (errorSwitch == ErrorSwitch::sense)? true : false;
    *dispSwAccUp = (displaySwitch == DisplaySwitch::upperAcc)? true : false;
    *dispSwAccLow = (displaySwitch == DisplaySwitch::lowerAcc)? true : false;
    *dispSwDist = (displaySwitch == DisplaySwitch::distributor)? true : false;
    *dispSwPgmReg = (displaySwitch == DisplaySwitch::programStep)? true : false;
    *dispSwReadIn = (displaySwitch == DisplaySwitch::readIn)? true : false;
    *dispSwReadOut = (displaySwitch == DisplaySwitch::readOut)? true : false;
    *addrSwitchTh = addrSelectSwitches[dd4];
    *addrSwitchH  = addrSelectSwitches[dd3];
    *addrSwitchT  = addrSelectSwitches[dd2];
    *addrSwitchU  = addrSelectSwitches[dd1];
    *readInStorage = (displaySwitch == DisplaySwitch::readIn)? true : false;
    *readOutStorage = (displaySwitch == DisplaySwitch::readOut)? true : false;
    *storageControl = (displaySwitch != DisplaySwitch::readIn)? true : false;
    *runControl = (displaySwitch == DisplaySwitch::lowerAcc
                   || displaySwitch == DisplaySwitch::upperAcc
                   || displaySwitch == DisplaySwitch::distributor
                   || displaySwitch == DisplaySwitch::programStep)? true : false;
}

void ModuleOperatorCtl::cycleSeqA() {
    // Operator console state machine. Once started, a state sequence completes
    // (returns to the 'idle' state) atomically relative to the command processor.
    *currentCommandDebug = (int)currentCommand;
    switch (currentState) {
        case ConsoleState::idle:
            processCommands();
            break;
        case ConsoleState::programReset1:
            if (*digitNumber == dd10 && *upperWord) {
                *programReset = true;
                if (*ctlSwRun || *ctlSwAddrStop) {
                    *set8000 = true;
                } else {
                    *reset0000 = true;
                }
                nextState = ConsoleState::programReset2;
            }
            break;
        case ConsoleState::programReset2:
            if (*digitNumber == ddx && !*upperWord) {
                nextState = ConsoleState::programReset3;
            }
            break;
        case ConsoleState::programReset3:
            if (*digitNumber == ddx && !*upperWord) {
                *programReset = false;
                *set8000 = false;
                *reset0000 = false;
                nextState = ConsoleState::idle;
            }
            break;
        case ConsoleState::accReset1:
            if (*digitNumber == dd10 && *upperWord) {
                *accReset = true;
                nextState = ConsoleState::accReset2;
            }
            break;
        case ConsoleState::accReset2:
            if (*digitNumber == ddx && !*upperWord) {
                nextState = ConsoleState::accReset3;
            }
            break;
        case ConsoleState::accReset3:
            if (*digitNumber == ddx && !*upperWord) {
                *accReset = false;
                nextState = ConsoleState::idle;
            }
            break;
        case ConsoleState::errorReset1:
            if (*digitNumber == dd10 && *upperWord) {
                *errorReset = true;
                nextState = ConsoleState::errorReset2;
            }
            break;
        case ConsoleState::errorReset2:
            if (*digitNumber == ddx && !*upperWord) {
                nextState = ConsoleState::errorReset3;
            }
            break;
        case ConsoleState::errorReset3:
            if (*digitNumber == ddx) {
                *errorReset = false;
                nextState = ConsoleState::idle;
            }
            break;
        case ConsoleState::errorSenseReset1:
            if (*digitNumber == dd10 && *upperWord) {
                *errorSenseReset = true;
                nextState = ConsoleState::errorSenseReset2;
            }
            break;
        case ConsoleState::errorSenseReset2:
            if (*digitNumber == ddx && !*upperWord) {
                nextState = ConsoleState::errorSenseReset3;
            }
            break;
        case ConsoleState::errorSenseReset3:
            if (*digitNumber == ddx) {
                *errorSenseReset = false;
                nextState = ConsoleState::idle;
            }
            break;
        case ConsoleState::xferKey1:
            *transferKey = true;
            nextState = ConsoleState::xferKey2;
            break;
        case ConsoleState::xferKey2:
            *transferKey = false;
            nextState = ConsoleState::idle;
            break;
        case ConsoleState::programStartKey1:
            if (*digitNumber == dd10 && *upperWord) {
                *programStart = true;
                nextState = ConsoleState::programStartKey2;
            }
            break;
        case ConsoleState::programStartKey2:
            if (*digitNumber == ddx && !*upperWord) {
                nextState = ConsoleState::programStartKey3;
            }
            break;
        case ConsoleState::programStartKey3:
            if (*digitNumber == ddx) {
                *programStart = false;
                nextState = ConsoleState::idle;
            }
            break;
    }
    *currentStateDebug = (int)currentState;
    *nextStateDebug    = (int)nextState;
    currentState       = nextState;
    *dataSwitchesOut   = storageEntrySwitches[*digitNumber];
}

void ModuleOperatorCtl::cycleClockA() {
    accReset->cycle();
    programReset->cycle();
    errorReset->cycle();
    errorSenseReset->cycle();
    set8000->cycle();
    reset0000->cycle();
    programStart->cycle();
    programStop->cycle();
    
    busy->cycle();
    
    currentStateDebug->cycle();
    nextStateDebug->cycle();
    currentCommandDebug->cycle();
    nextCommandDebug->cycle();
    dataSwitchesOut->cycle();
}

std::vector<SimDispatchable> ModuleOperatorCtl::getDispatchables() {
    std::vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    return dispatchables;
}

void ModuleOperatorCtl::resetControls() {
    // console switches to their reset positions
    programSwitch   = ProgramSwitch::run;
    halfCycleSwitch = HalfCycleSwitch::run;
    controlSwitch   = ControlSwitch::run;
    displaySwitch   = DisplaySwitch::lowerAcc;
    overflowSwitch  = OverflowSwitch::stop;
    errorSwitch     = ErrorSwitch::stop;
    storageEntrySwitches.plusZero();
    addrSelectSwitches.signedDecimal("+8000");
    *addrSwitchTh = addrSelectSwitches[dd4];
    *addrSwitchH  = addrSelectSwitches[dd3];
    *addrSwitchT  = addrSelectSwitches[dd2];
    *addrSwitchU  = addrSelectSwitches[dd1];
}

void ModuleOperatorCtl::reset() {
    resetControls();
    
    currentCommand = ConsoleCommand::none;
    nextCommand    = ConsoleCommand::none;
    currentState   = ConsoleState::idle;
    nextState      = ConsoleState::idle;
    
    doPowerOnReset = true;
    doResetConsole = false;
    doErrReset = false;
    doErrSenseReset = false;
    doPgmReset = false;
    doAccReset = false;
    //busy->set(true);
    
    *currentCommandDebug = (int)currentCommand;
    *nextCommandDebug    = (int)nextCommand;
    *currentStateDebug   = (int)currentState;
    *nextStateDebug      = (int)nextState;
}
