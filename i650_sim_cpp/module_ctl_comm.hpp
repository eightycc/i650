//
//  module_ctl_comm.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_ctl_comm_hpp
#define module_ctl_comm_hpp

class ModuleCtlComm : public SimModule {
    // Input wires used throughout gathered here (grouped by source module)
    PortInInt *digitNum;
    PortInBool *lowerWord;
    
    PortInBool *pgmReset;
    PortInBool *errorReset;
    PortInBool *dispSwReadOut;
    PortInBool *dispSwReadIn;
    PortInBool *hcSwHalf;
    PortInBool *startKey;
    PortInBool *stopKey;
    PortInBool *stopAddr; // console address compare stop

    PortInBiq  *opcodeT;
    PortInBiq  *opcodeU;
    PortInBool *addrNo800x;
    
    
    // Restart control
    PortInBool *pgmRestart;
    PortInBool *arithRestart;
    PortInBool *decodeRestarts;
    PortInBool *addrInvalid;
    PortInBool *opStop; // 01 Stop
    POutBoolReg *restartA;
    POutBoolReg *restartB;
    
    // I/D alternation
    PortInBool  *runControl; // display switch in one of: lowAdd, upAcc, dist, ps
    PortInBool  *useDforI;
    POutBoolReg *iAlt;
    POutBoolReg *dAlt;
    POutBool *iControl;
    POutBool *dControl;
    POutBool *readPunchDCtl;
    
    // Run control
    PortInBool *errSenseRestart;    // applied directly to run latch
    PortInBool *errStop;
    PortInBool *distBack;
//    PortInBool
    FFBoolEdgeReg *startStopLatch;
    FFBoolEdgeReg *runLatch;
    
    // Read-in / read-out control
    POutBoolReg *manReadOutStore;
    FFBoolReg *readInOperand; // this is "[10's & 60's] 8001 -- "D" Control"
    FFBoolEdgeReg *enableReadIn;
    
    // Interlocks
    POutBoolReg *opInterlock;
    POutBoolReg *singleInterlock;
    
    // Decode Store Ops (20..24)
    FFBool *op20_24_d;
    FFBool *op20_24_d_intlk;
    FFBool *op20_start;
    FFBool *op21_start;
    FFBool *op22_start;
    FFBool *op23_start;
    FFBool *op20_stop;  // STL (lower)
    FFBool *op21_stop;  // STU (upper)
    FFBool *op22_stop;  // STDA (d-addr)
    FFBool *op23_stop;  // STIA (i-addr)
    FFBool *op24;       // STD
    
  public:
    ModuleCtlComm(SimModelPtr, const std::string&, SimObjPtr);
    virtual std::vector<SimDispatchable> getDispatchables();
    virtual void cycleComboA();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual void cycleSeqD();
    virtual void cycleClockD();
};

#endif /* module_ctl_comm_hpp */
