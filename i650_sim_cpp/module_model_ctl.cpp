//
//  module_model_ctl.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/12/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"

using namespace std;

ModelController::ModelController(CommonTestControl* tc) {
    // Populate the model
    model      = new SimModel();
    top        = new ModuleTop(model);
    // Append our controller, initialize and reset model
    controller = new ModuleModelCtl(model, "sc", top);
    new SimWire(model, "sc_command_out", top, "sc/command_out", "oc/command_in");
    new SimWire(model, "sc_operand_out", top, "sc/operand_out", "oc/command_op");
    new SimWire(model, "sc_sw_set_out", top, "sc/sw_set_out", "oc/command_sw");
    new SimWire(model, "oc_busy", top, "oc/busy", "sc/busy");
    // Insert test controller, if applicable
    if (tc) {
        testControl = tc;
        testControl->initialize(model, top);
    }
    model->resetModel();
    // Locate key modules in the design
    auto obj   = model->findModule("oc", top);
    opControl  = dynamic_cast<ModuleOperatorCtl *>(obj);
    obj = model->findModule("acc", top);
    acc = dynamic_cast<ModuleAccumulator *>(obj);
    obj = model->findModule("ar", top);
    ar  = dynamic_cast<ModuleAddrReg *>(obj);
    obj = model->findModule("gs", top);
    gs  = dynamic_cast<ModuleGenStore *>(obj);
    obj = model->findModule("dist", top);
    dist = dynamic_cast<ModuleDistributor *>(obj);
    obj = model->findModule("time", top);
    time = dynamic_cast<ModuleTiming *>(obj);
}
ModelController::~ModelController() {
    delete model;
}
void ModelController::reset() {
    model->resetModel();
}
void ModelController::cycle() {
    model->cycleModel();
}
bool ModelController::cycleUntilNotBusy(int timeout) {
    while (controller->isBusy()) {
        model->cycleModel();
        if (timeout-- <= 0) {
            return false;
        }
    }
    return true;
}
bool ModelController::cycleUntilBusy(int timeout) {
    while (!controller->isBusy()) {
        model->cycleModel();
        if (timeout-- <= 0) {
            return false;
        }
    }
    return true;
}
void ModelController::cycleUntil(int digit) {
    while (getDigitNumber() != digit) {
        model->cycleModel();
    }
}
void ModelController::cycleUntil(bool upper, int digit) {
    while (getDigitNumber() != digit || getUpperWord() != upper) {
        model->cycleModel();
    }
}
void ModelController::beginTracing(const string& fn) {
    model->beginTracing(fn);
}
void ModelController::endTracing() {
    model->endTracing();
}
void ModelController::pauseTracing() {
    model->pauseTracing();
}
void ModelController::resumeTracing() {
    model->resumeTracing();
}
bool ModelController::doCommand(ConsoleCommand command) {
    controller->sendCommand(command);
    if (!cycleUntilBusy()) {
        cout << "*** Timeout waiting for busy to rise after command issue. ***" << endl;
        return false;
    }
    if (!cycleUntilNotBusy()) {
        cout << "*** Timeout waiting for busy to drop after command issue. ***" << endl;
        return false;
    }
    return true;
}
bool ModelController::doCommand(ConsoleCommand command, const BiqWord& operand) {
    controller->sendCommand(command, operand);
    if (!cycleUntilBusy()) {
        cout << "*** Timeout waiting for busy to rise after command issue. ***" << endl;
        return false;
    }
    if (!cycleUntilNotBusy()) {
        cout << "*** Timeout waiting for busy to drop after command issue. ***" << endl;
        return false;
    }
    return true;
}
bool ModelController::doCommand(ConsoleCommand command, int switchSetting) {
    controller->sendCommand(command, switchSetting);
    if (!cycleUntilBusy()) {
        cout << "*** Timeout waiting for busy to rise after command issue. ***" << endl;
        return false;
    }
    if (!cycleUntilNotBusy()) {
        cout << "*** Timeout waiting for busy to drop after command issue. ***" << endl;
        return false;
    }
    return true;
}

void ModelController::printConnectionReport(ostream& s) const {
    model->printConnectionReport(s);
}

// Load drum from fn (implicit open/close), returns true on success.
// Dump file format is 1 byte per digit, 12 digits per word, in
// ascending word order and ascending digit number (i.e., DX..D10)
bool ModelController::loadDrum(const string& fn) {
    if (!gs) {
        return false;
    }
    return gs->loadDrum(fn);
}
bool ModelController::dumpDrum(const string& fn) {
    if (!gs) {
        return false;
    }
    return gs->dumpDrum(fn);
}
const BiqWord& ModelController::getDrum(int addr) {
    if (!gs) {
        return biqWordBlank;
    }
    return gs->readDrum(addr);
}
void ModelController::setDrum(int addr, const BiqWord& v) {
    if (!gs) {
        return;
    }
    gs->writeDrum(addr, v);
}
const BiqWord& ModelController::getAccUpper() const {
    if (!acc) {
        return biqWordBlank;
    }
    return acc->readUpper();
}
void ModelController::setAccUpper(const BiqWord& v) {
    if (!acc) {
        return;
    }
    acc->writeUpper(v);
}
const BiqWord& ModelController::getAccLower() const {
    if (!acc) {
        return biqWordBlank;
    }
    return acc->readLower();
}
void ModelController::setAccLower(const BiqWord& v) {
    if (!acc) {
        return;
    }
    acc->writeLower(v);
}
const BiqWord& ModelController::getAddrReg() const {
    if (!ar) {
        return biqWordBlank;
    }
    return ar->readAddrReg();
}
void ModelController::setAddrReg(const BiqWord& v) {
    if (!ar) {
        return;
    }
    ar->writeAddrReg(v);
}
const BiqWord& ModelController::getDist() const {
    if (!dist) {
        return biqWordBlank;
    }
    return dist->read();
}
void ModelController::setDist(const BiqWord& v) {
    if (!dist) {
        return;
    }
    dist->write(v);
}
int ModelController::getDigitNumber() const {
    if (!time) {
        return 0;
    }
    return time->getDigitNumber();
}
bool ModelController::getUpperWord() const {
    if (!time) {
        return 0;
    }
    return time->getUpperWord();
}


ModuleModelCtl::ModuleModelCtl(SimModelPtr m, const string& n, SimObjPtr p)
  : SimModule(m, n, p)
{
    commandOut       = new POutIntReg(m, "command_out", this, 0);
    operandOut       = new POutBiqWordReg
                                     (m, "operand_out", this, biqWordBlank);
    switchSettingOut = new POutIntReg(m, "sw_set_out", this, 0);
    busy             = new PortInBool(m, "busy", this);
}

void ModuleModelCtl::reset() {
    *commandOut = 0;
    *operandOut = biqWordBlank;
    *switchSettingOut = 0;
    pendingCommand = (int)ConsoleCommand::none;
    pendingOperand = biqWordBlank;
    pendingSwitchSetting = 0;
}

void ModuleModelCtl::cycleSeqA() {
    *commandOut = pendingCommand;
    *operandOut = pendingOperand;
    *switchSettingOut = pendingSwitchSetting;
    pendingCommand = (int)ConsoleCommand::none;
    pendingOperand = biqWordBlank;
    pendingSwitchSetting = 0;
}
void ModuleModelCtl::cycleClockA() {
    commandOut->cycle();
    operandOut->cycle();
    switchSettingOut->cycle();
}
void ModuleModelCtl::sendCommand(ConsoleCommand command) {
    pendingCommand = (int)command;
    pendingOperand = biqWordBlank;
    pendingSwitchSetting = 0;
}
void ModuleModelCtl::sendCommand(ConsoleCommand command, const BiqWord& operand) {
    pendingCommand = (int)command;
    pendingOperand = operand;
    pendingSwitchSetting = 0;
}
void ModuleModelCtl::sendCommand(ConsoleCommand command, int switchSetting) {
    pendingCommand = (int)command;
    pendingOperand = biqWordBlank;
    pendingSwitchSetting = switchSetting;
}

vector<SimDispatchable> ModuleModelCtl::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aSeq, &SimModule::cycleSeqA));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::aClock, &SimModule::cycleClockA));
    return dispatchables;
}
