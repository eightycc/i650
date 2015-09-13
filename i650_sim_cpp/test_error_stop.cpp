//
//  test_error_stop.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_error_stop.hpp"

using namespace std;

ErrorStopTestCtl::ErrorStopTestCtl()
{
    modelControl = new ModelController(this);
}

void ErrorStopTestCtl::runTest() {
    cout << "///// Starting estst1 /////" << endl;
    auto esTest = dynamic_cast<TestErrorStop *>(test);
    modelControl->beginTracing("/Users/rabeles/Development/i650_sim_cpp/i650_sim_cpp/GTKWave/estst1.lxt");
    // Wait for completion of power-on reset
    modelControl->cycle();
    modelControl->cycleUntilNotBusy();
    
    // Set error switch to "stop"
    modelControl->doCommand(ConsoleCommand::setErrorSwitch, (int)ErrorSwitch::stop);
    // Inject a clocking error
    modelControl->cycleUntil(false, ddx);
    *esTest->clockError = true;
    modelControl->cycle();
    *esTest->clockError = false;
    modelControl->cycleUntil(false, ddx);
    // Inject another clocking error, this time with error switch to "sense"
    modelControl->doCommand(ConsoleCommand::pressErrorResetKey);
    modelControl->doCommand(ConsoleCommand::setErrorSwitch, (int)ErrorSwitch::sense);
    modelControl->cycleUntil(false, ddx);
    *esTest->clockError = true;
    modelControl->cycle();
    *esTest->clockError = false;
    for (int i = 0; i < 600; i++)
        modelControl->cycle();

    // Run through a complete even/odd word time
    modelControl->cycleUntil(false, ddx);
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    
    modelControl->endTracing();
    modelControl->printConnectionReport(cout);
    cout << "///// Ending estst1   /////" << endl;
}

void ErrorStopTestCtl::initialize(SimModelPtr m, SimModulePtr top) {
    CommonTestControl::initialize(m, top);
    test = new TestErrorStop(m, "estst", top);
    new SimWire(m, "estst_clock_error", top, "estst/clock_error", "es/clock_error", true);
    new SimWire(m, "estst_err_stop_sig", top, "estst/err_stop_sig", "es/err_stop_sig", true);
}

TestErrorStop::TestErrorStop(SimModelPtr m, const string& nm, SimModulePtr p)
  : CommonTestModule(m, nm, p)
{
    clockError = new POutBool(m, "clock_error", this, false);
    errorStopSignal = new POutBool(m, "err_stop_sig", this, false);
}