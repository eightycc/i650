//
//  module_error_stop.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_error_stop_hpp
#define module_error_stop_hpp

class ModuleErrorStop : public SimModule {
    enum class RestartState {
        idle = 0,
        sense1, sense2, sense3, sense4, sense5,
        stop1, stop2
    };
    PortInInt *digitNum;
    PortInBool *lowerWord;
    PortInBool *errSwSense;
    PortInBool *errorReset;
    PortInBool *errorSenseReset;
    PortInBool *clockError;
    PortInBool *errorStopSignal;
    PortInBool *ocBusy;
    
    POutBoolReg *errorSense;
    POutBoolReg *errorStop;
    POutBoolReg *errorSenseRestart;
    POutBoolReg *restartReset;
    POutBoolReg *turnOffRunLatch;

    FFIntReg *restartState;    // state of restart reset fsm
    int timeOut;
    
  public:
    ModuleErrorStop(SimModelPtr, const std::string&, SimObjPtr);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleSeqB();
    virtual void cycleClockB();
};

#endif /* module_error_stop_hpp */
