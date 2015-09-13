//
//  test_ctl_comm.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 9/4/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_ctl_comm.hpp"

using namespace std;

CtlCommTestControl::CtlCommTestControl()
{
    modelControl = new ModelController(this);
}

void CtlCommTestControl::initialize(SimModelPtr m, SimModulePtr top) {
    CommonTestControl::initialize(m, top); // mandatory invokation of common test initialization
    test = new TestCtlComm(m, "cctest", top);
    new SimWire(m, "cctest_pgm_restart", top, "cctest/pgm_restart", "cc/pgm_restart");
    new SimWire(m, "cctest_arith_restart", top, "cctest/arith_restart", "cc/arith_restart");
    new SimWire(m, "cctest_decode_restarts", top, "cctest/decode_restarts", "cc/decode_restarts");
    new SimWire(m, "cctest_use_d_for_i", top, "cctest/use_d_for_i", "cc/use_d_for_i");
    
    
}

void CtlCommTestControl::runTest() {
    cout << "///// Starting cctst1 /////" << endl;
    auto ccTest = dynamic_cast<TestCtlComm *>(test);
    modelControl->beginTracing("/Users/rabeles/Development/i650_sim_cpp/i650_sim_cpp/GTKWave/cctst1.lxt");
    // Wait for completion of power-on reset
    modelControl->cycle();
    modelControl->cycleUntilNotBusy();
    // Power-on reset is now complete, TODO: make this waitForPowerOn()
    
    // Fill drum with test pattern
    fillDrum();
    
    // Press program start key
    doCommand(ConsoleCommand::pressProgramStartKey);
    
    // Program restart (error sense)
    *ccTest->pgmRestart = true;
    modelControl->cycle();
    *ccTest->pgmRestart = false;
    
    // Delay 10 cycles and then strobe program restart
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    modelControl->cycle();
    *ccTest->pgmRestart = true;
    modelControl->cycle();
    *ccTest->pgmRestart = false;
    
#if 0
    
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
#endif
    // Run through a complete even/odd word time
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    
    modelControl->endTracing();
    //modelControl->printConnectionReport(cout);
    cout << "///// Ending cctst1   /////" << endl;
    //topModule->printTree(cout);
}

TestCtlComm::TestCtlComm(SimModelPtr m, const string& nm, SimModulePtr p)
: CommonTestModule(m, nm, p)
{
    pgmRestart = new POutBool(m, "pgm_restart", this);
    arithRestart = new POutBool(m, "arith_restart", this);
    decodeRestarts = new POutBool(m, "decode_restarts", this);
    useDforI = new POutBool(m, "use_d_for_i", this);
}
