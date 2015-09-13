//
//  module_operator_ctl.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/8/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_operator_ctl_hpp
#define module_operator_ctl_hpp

// Operator console switches
enum class ProgramSwitch   { stop, run };
enum class HalfCycleSwitch { run, half };
enum class ControlSwitch   { addrStop, run, manual };
enum class DisplaySwitch   { lowerAcc, upperAcc, distributor,
                             programStep, readIn, readOut };
enum class OverflowSwitch  { stop, sense };
enum class ErrorSwitch     { stop, sense };

// Operator console commands
enum class ConsoleCommand {
    none,
    
    // set operator panel switches
    setProgramSwitch, setHalfCycleSwitch, setControlSwitch, setDisplaySwitch,
    setOverflowSwitch, setErrorSwitch, setStorageEntrySwitches,
    setAddressSelectSwitches,
    
    // press operator panel keys
    pressTransferKey, pressProgramStartKey, pressProgramStopKey,
    pressProgramResetKey, pressComputerResetKey, pressAccResetKey,
    pressErrorResetKey, pressErrorSenseResetKey,
    
    // extra commands
    resetConsole
};

class ModuleOperatorCtl : public SimModule {
    PortInInt  *digitNumber;
    PortInInt  *wordNumber;
    PortInBool *upperWord;
    PortInInt  *sectorNumber;
    PortInInt  *earlyDigitNumber;
    PortInBiq  *selectedStore;
    PortInBiq  *accOntime;
    PortInBiq  *pgmStepOntime;
    PortInBool *restartReset;
    
    // Reset and control signals
    POutBoolReg *errorReset;
    POutBoolReg *errorSenseReset;
    POutBoolReg *programReset;
    POutBoolReg *accReset;
    POutBoolReg *set8000;
    POutBoolReg *reset0000;
    POutBool *runControl;
    POutBool *storageControl;
    POutBoolReg *halfCycleOrProgramStop;
    POutBoolReg *programStart;
    POutBoolReg *programStop;
    POutBool *readInStorage;
    POutBool *readOutStorage;
    
    // Console address and data switch outputs
    POutBiq *addrSwitchTh;
    POutBiq *addrSwitchH;
    POutBiq *addrSwitchT;
    POutBiq *addrSwitchU;
    POutBiqReg *dataSwitchesOut;
    POutBool *pgmSwStop;
    POutBool *pgmSwRun;
    POutBool *hcSwRun;
    POutBool *hcSwHalf;
    POutBool *ctlSwAddrStop;
    POutBool *ctlSwRun;
    POutBool *ctlSwMan;
    POutBool *ovflwSwStop;
    POutBool *ovflwSwSense;
    POutBool *errSwStop;
    POutBool *errSwSense;
    POutBool *dispSwAccUp;
    POutBool *dispSwAccLow;
    POutBool *dispSwDist;
    POutBool *dispSwPgmReg;
    POutBool *dispSwReadIn;
    POutBool *dispSwReadOut;
    POutBool *transferKey;
    
    // External interface wires
    PortInInt   *commandIn;
    PortInBiqWord *commandOperand;
    PortInInt   *commandSwitchSetting;
    POutBoolReg *busy;
    
    // Debug wires
    POutIntReg    *currentStateDebug;
    POutIntReg    *nextStateDebug;
    POutIntReg    *currentCommandDebug;
    POutIntReg    *nextCommandDebug;
    
    // Operator console switches
    ProgramSwitch   programSwitch;
    HalfCycleSwitch halfCycleSwitch;
    ControlSwitch   controlSwitch;
    DisplaySwitch   displaySwitch;
    OverflowSwitch  overflowSwitch;
    ErrorSwitch     errorSwitch;
    BiqWord         storageEntrySwitches;
    BiqWord         addrSelectSwitches; // address in D4..D1
    
    // Current operator console command
    ConsoleCommand currentCommand, nextCommand;
    int currentSwSetting, nextSwSetting;
    BiqWord currentOperand, nextOperand;
    
    // Operator console machine states
    enum class ConsoleState {
        idle,
        programReset1, programReset2, programReset3,
        accReset1, accReset2, accReset3,
        errorReset1, errorReset2, errorReset3,
        errorSenseReset1, errorSenseReset2, errorSenseReset3,
        xferKey1, xferKey2,
        programStartKey1, programStartKey2, programStartKey3
        /*
        resetConsole1, resetConsole2,
        programReset1, programReset2,
        accReset1, accReset2,
        errorReset1, errorReset2,
        errorSenseReset1, errorSenseReset2,
        transferKey1, transferKey2,
        programStartKey1, programStartKey2,
        programStopKey1, programStopKey2
        */
    } currentState, nextState;
    
    bool initialized = false;
    
    // Pending work flags
    bool doPowerOnReset = false;
    bool doResetConsole = false;
    bool doErrReset = false;
    bool doErrSenseReset = false;
    bool doPgmReset = false;
    bool doAccReset = false;
    
    void resetControls();
    void processCommands();
    void refreshSwitches();
  public:
    ModuleOperatorCtl(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void reset();
};

#endif /* module_operator_ctl_hpp */
