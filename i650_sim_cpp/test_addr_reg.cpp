//
//  test_addr_reg.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/22/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_addr_reg.hpp"

using namespace std;

AddrRegTestControl::AddrRegTestControl()
{
    modelControl = new ModelController(this);
}

void AddrRegTestControl::initialize(SimModelPtr m, SimModulePtr top) {
    CommonTestControl::initialize(m, top); // mandatory invokation of common test initialization
    test = new TestAddrReg(m, "artest", top);
    new SimWire(m, "artest_dynamic_hit", top, "ar/addr_hit", "artest/dynamic_hit", true);
    new SimWire(m, "artest_restart_a", top, "artest/restart_a", "ar/restart_a", true);
    new SimWire(m, "artest_tlu_band_chg", top, "artest/tlu_band_chg", "ar/tlu_band_chg", true);
    new SimWire(m, "artest_ri_pgm_step", top, "artest/ri_pgm_step", "ar/read_in_ar", true);
}

void AddrRegTestControl::runTest() {
    cout << "///// Starting artst1 /////" << endl;
    auto arTest = dynamic_cast<TestAddrReg *>(test);
    modelControl->beginTracing("/Users/rabeles/Development/i650_sim_cpp/i650_sim_cpp/GTKWave/artst1.lxt");
    // Wait for completion of power-on reset
    modelControl->cycle();
    modelControl->cycleUntilNotBusy();
    // Power-on reset is now complete, TODO: make this waitForPowerOn()
    
    // Fill drum with test pattern
    fillDrum();
    // Test oc->ar path on oc tranfer key press
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("1234"));
    doCommand(ConsoleCommand::setControlSwitch, (int)ControlSwitch::manual);
    doCommand(ConsoleCommand::pressTransferKey);
    
    // Transfer invalid address into ar
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("9999"));
    doCommand(ConsoleCommand::pressTransferKey);
    // Transfer a valid address
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("1984"));
    doCommand(ConsoleCommand::pressTransferKey);
    // Error reset
    doCommand(ConsoleCommand::pressErrorResetKey);
    // Test address decode
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("8000"));
    doCommand(ConsoleCommand::pressTransferKey);
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("8001"));
    doCommand(ConsoleCommand::pressTransferKey);
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("8002"));
    doCommand(ConsoleCommand::pressTransferKey);
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("8003"));
    doCommand(ConsoleCommand::pressTransferKey);
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("1967"));
    doCommand(ConsoleCommand::pressTransferKey);
    // Wait for dynamic hit on 1967
    int timeOut = 600;
    while (!*arTest->dynamicHit) {
        modelControl->cycle();
        if (--timeOut <= 0) {
            cout << "*** Timeout waiting for dynamicHit. ***" << endl;
            break;
        }
    }
    // Try dynamic hit on 1812
    doCommand(ConsoleCommand::setAddressSelectSwitches, BiqWord("1812"));
    doCommand(ConsoleCommand::pressTransferKey);
    timeOut = 600;
    while (!*arTest->dynamicHit) {
        modelControl->cycle();
        if (--timeOut <= 0) {
            cout << "*** Timeout waiting for dynamicHit. ***" << endl;
            break;
        }
    }
#if 0
    while (!*arTest->dynamicHit) {
        modelControl->cycle();
        if (--timeOut <= 0) {
            cout << "*** Timeout waiting for dynamicHit. ***" << endl;
            break;
        }
    }
#endif
    // Run through a complete even/odd word time
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    
    modelControl->endTracing();
    //modelControl->printConnectionReport(cout);
    cout << "///// Ending artst1   /////" << endl;
    //topModule->printTree(cout);
}

TestAddrReg::TestAddrReg(SimModelPtr m, const string& nm, SimModulePtr p)
  : CommonTestModule(m, nm, p)
{
    dynamicHit = new PortInBool(m, "dynamic_hit", this);
    restartA = new POutBool(m, "restart_a", this, false);
    tluBandChange = new POutBool(m, "tlu_band_chg", this, false);
    readInPgmStep = new POutBool(m, "ri_pgm_step", this, false);
}
