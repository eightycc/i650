//
//  module_model_ctl.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/12/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef module_model_ctl_hpp
#define module_model_ctl_hpp

class ModuleModelCtl;
class CommonTestControl;

class ModelController {
    SimModelPtr        model = nullptr;
    ModuleTop*         top   = nullptr;
    ModuleModelCtl*    controller = nullptr;
    ModuleOperatorCtl* opControl = nullptr;
    
    ModuleTiming*      time = nullptr;
    ModuleAccumulator* acc = nullptr;
    ModuleAddrReg*     ar  = nullptr;
    ModuleGenStore*    gs  = nullptr;
    ModuleDistributor* dist = nullptr;
    
    CommonTestControl*    testControl = nullptr; // optional test fixture
    
  public:
    ModelController(CommonTestControl* tc = nullptr);
    virtual ~ModelController();
    
    void reset();
    void cycle();
    bool cycleUntilNotBusy(int timeout = 600);
    bool cycleUntilBusy(int timeout = 600);
    void cycleUntil(int digit);
    void cycleUntil(bool upper, int digit);
    void beginTracing(const std::string& fn);
    void endTracing();
    void pauseTracing();
    void resumeTracing();
    bool doCommand(ConsoleCommand command);
    bool doCommand(ConsoleCommand command, const BiqWord& operand);
    bool doCommand(ConsoleCommand command, int switchSetting);
    void printConnectionReport(std::ostream& s) const;
    
    int getDigitNumber() const;
    bool getUpperWord() const;

    const BiqWord& getAccUpper() const;
    void setAccUpper(const BiqWord& v);
    const BiqWord& getAccLower() const;
    void setAccLower(const BiqWord& v);
    
    const BiqWord& getAddrReg() const;
    void setAddrReg(const BiqWord&);
    
    const BiqWord& getDist() const;
    void setDist(const BiqWord& v);
    
    bool loadDrum(const std::string& fn);
    bool dumpDrum(const std::string& fn);
    const BiqWord& getDrum(int addr);
    void setDrum(int addr, const BiqWord& v);
};

// This is an example of an interface between the design under simulation
// and a controller. Added when ModelController is initiated.
class ModuleModelCtl : public SimModule {
    POutIntReg          *commandOut;
    POutBiqWordReg      *operandOut;
    POutIntReg          *switchSettingOut;
    PortInBool          *busy;
    
    int pendingCommand;
    BiqWord pendingOperand;
    int pendingSwitchSetting;
    
  public:
    ModuleModelCtl(SimModelPtr m, const std::string& n, SimObjPtr p);
    virtual void reset();
    virtual void cycleSeqA();
    virtual void cycleClockA();
    virtual std::vector<SimDispatchable> getDispatchables();

    bool isBusy() const {return (bool)*busy;}
    void sendCommand(ConsoleCommand command);
    void sendCommand(ConsoleCommand command, const BiqWord& operand);
    void sendCommand(ConsoleCommand command, int switchSetting);
};

#endif /* module_model_ctl_hpp */
