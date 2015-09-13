//
//  test_operator_ctl.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/20/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_operator_ctl.hpp"

using namespace std;

OperatorTestCtl::OperatorTestCtl()
{
    modelControl = new ModelController(this);
}

void OperatorTestCtl::initialize(SimModelPtr m, SimModulePtr top) {
    CommonTestControl::initialize(m, top); // mandatory invokation of common test initialization
    test = new TestOperatorCtl(m, "octst", top);
    new SimWire(m, "octst_restart_reset", top, "octst/restart_reset", "oc/restart_reset", true);
}

void OperatorTestCtl::testSwitch(ConsoleCommand c, vector<int> v) {
    for (auto setting : v) {
        modelControl->doCommand(c, setting);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
    for (auto setting : v) {
        modelControl->doCommand(c, setting);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
}
void OperatorTestCtl::testAddrSwitches(vector<BiqWord> v) {
    for (auto testWord : v) {
        modelControl->doCommand(ConsoleCommand::setAddressSelectSwitches, testWord);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
    for (auto testWord : v) {
        modelControl->doCommand(ConsoleCommand::setAddressSelectSwitches, testWord);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
}
void OperatorTestCtl::testDataSwitches(vector<BiqWord> v) {
    for (auto testWord : v) {
        modelControl->doCommand(ConsoleCommand::setStorageEntrySwitches, testWord);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
    for (auto testWord : v) {
        modelControl->doCommand(ConsoleCommand::setStorageEntrySwitches, testWord);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
}
void OperatorTestCtl::testButton(ConsoleCommand c) {
    for (int i = 0; i < 2; i++) {
        modelControl->doCommand(c);
        modelControl->cycleUntilNotBusy();
        modelControl->cycleUntil(false, ddx);
        modelControl->cycle();
        modelControl->cycleUntil(false, ddx);
    }
}

void OperatorTestCtl::runTest() {
    cout << "///// Starting octst1 /////" << endl;
    modelControl->beginTracing("/Users/rabeles/Development/i650_sim_cpp/i650_sim_cpp/GTKWave/octst1.lxt");
    // Wait for completion of power-on reset
    modelControl->cycle();
    modelControl->cycleUntilNotBusy();
    // Test mode switches
    testSwitch(ConsoleCommand::setProgramSwitch, {
        (int)ProgramSwitch::stop,
        (int)ProgramSwitch::run
    });
    testSwitch(ConsoleCommand::setHalfCycleSwitch, {
        (int)HalfCycleSwitch::run,
        (int)HalfCycleSwitch::half
    });
    testSwitch(ConsoleCommand::setControlSwitch, {
        (int)ControlSwitch::run,
        (int)ControlSwitch::addrStop,
        (int)ControlSwitch::manual
    });
    testSwitch(ConsoleCommand::setOverflowSwitch, {
        (int)OverflowSwitch::stop,
        (int)OverflowSwitch::sense
    });
    testSwitch(ConsoleCommand::setErrorSwitch, {
        (int)ErrorSwitch::stop,
        (int)ErrorSwitch::sense
    });
    testSwitch(ConsoleCommand::setDisplaySwitch, {
        (int)DisplaySwitch::lowerAcc,
        (int)DisplaySwitch::upperAcc,
        (int)DisplaySwitch::distributor,
        (int)DisplaySwitch::programStep,
        (int)DisplaySwitch::readIn,
        (int)DisplaySwitch::readOut
    });
    testAddrSwitches({
        BiqWord("0000"),
        BiqWord("1234"),
        BiqWord("8002")
    });
    testDataSwitches({
        BiqWord("+0123456789"),
        BiqWord("-9876543210"),
        BiqWord("+0")
    });
    testButton(ConsoleCommand::pressAccResetKey);
    testButton(ConsoleCommand::pressComputerResetKey);
    testButton(ConsoleCommand::pressErrorResetKey);
    testButton(ConsoleCommand::pressErrorSenseResetKey);
    testButton(ConsoleCommand::pressProgramResetKey);
    testButton(ConsoleCommand::pressTransferKey);
    //testButton(ConsoleCommand::pressProgramStopKey);
    //testButton(ConsoleCommand::pressProgramStartKey);
    testButton(ConsoleCommand::resetConsole);
    
    // TODO: Implement and test address compare stop
    
    // TODO: Test restart_reset
    
    // Run through a complete even/odd word time
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    
    modelControl->endTracing();
    //modelControl->printConnectionReport(cout);
    cout << "///// Ending octst1   /////" << endl;
}

TestOperatorCtl::TestOperatorCtl(SimModelPtr m, const string& nm, SimModulePtr p)
  : CommonTestModule(m, nm, p)
{
    restartReset = new POutBool(m, "restart_reset", this, false);
}