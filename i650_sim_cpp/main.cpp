//
//  main.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include <iostream>
#include <chrono>
#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_operator_ctl.hpp"
#include "test_addr_reg.hpp"
#include "test_adder.hpp"
#include "test_error_stop.hpp"
#include "test_ctl_comm.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    //testBiqDigit();
    //testBiqWord();
    //testPorts();
    
#if 0
    SimObject::resetStatics();
    ModuleTop mytop;
    cout << "*******************Newly created top_module" << endl;
    mytop.printTree(cout);
    SimObject::initModel();
    cout << "*******************Initialized" << endl;
    mytop.printTree(cout);
    SimObject::resetModel();
    cout << "*******************Reset" << endl;
    mytop.printTree(cout);
    
    SimObject::cycleModel();
    cout << "*******************Cycle 1" << endl;
    mytop.printTree(cout);
    
    SimObject::cycleModel();
    cout << "*******************Cycle 2" << endl;
    mytop.printTree(cout);
    
    SimObject::cycleModel();
    cout << "*******************Cycle 3" << endl;
    mytop.printTree(cout);
    
    SimObject::cycleModel();
    cout << "*******************Cycle 4" << endl;
    mytop.printTree(cout);
    SimModel  myModel;
    ModuleTop* myTop = new ModuleTop(&myModel);
    myModel.initModel();
    myModel.resetModel();
    cout << "*******************After reset" << endl;
    myTop->printTree(cout);
    myModel.cycleModel();
    cout << "*******************After cycle 0" << endl;
    myTop->printTree(cout);
    myModel.cycleModel();
    cout << "*******************After cycle 1" << endl;
    myTop->printTree(cout);
    myModel.resetModel();
    auto iterations = 10000;
    auto t = steady_clock::now();
    for (auto i = iterations; i > 0; i--) {
        myModel.cycleModel();
    }
    auto d = steady_clock::now() - t;
    //mytop.printTree(cout);
    cout << "Total time: " << duration_cast<nanoseconds>(d).count() << "ns"
         << ", Per iteration: " << duration_cast<nanoseconds>(d).count() / iterations << "ns"
         << endl;
    myModel.resetModel();
    myModel.beginTracing("/Users/rabeles/Development/test1.lxt");
    t = steady_clock::now();
    for (int j = 0; j<iterations; j++) {
        myModel.cycleModel();
    }
    d = steady_clock::now() - t;
    cout << "Total time: " << duration_cast<nanoseconds>(d).count() << "ns"
         << ", Per iteration: " << duration_cast<nanoseconds>(d).count() / iterations << "ns"
         << endl;
    myModel.endTracing();
    myModel.resetModel();
    auto obj = myModel.lookupByFullName("module/top/acc");
    auto acc = dynamic_cast<ModuleAccumulator *>(obj);
    acc->inject(BiqWord("+3232323232"), BiqWord("-5454545454"));
    cout << *acc << endl;
    auto iterations = 48;
    myModel.beginTracing("/Users/rabeles/Development/test2.lxt");
    for (auto i = iterations; i > 0; i--) {
        myModel.cycleModel();
        cout << *acc << endl;
    }
    //myModel.endTracing();
    //myTop->printTree(cout);
    obj = myModel.lookupByFullName("module/top/gs");
    auto gs = dynamic_cast<ModuleGenStore *>(obj);
    cout << *gs;
    for (auto addr = 0; addr < 2000; addr++) {
        auto word = BiqWord("+9876543210");
        word.dAddr(addr);
        word.iAddr(addr);
        word.opCode(addr);
        gs->setWord(addr, word);
    }
    cout << *gs;
    iterations = 2000;
    for (auto i = iterations; i > 0; i--) {
        myModel.cycleModel();
    }
    cout << *gs;
    myModel.endTracing();
    
    myModel.resetModel();
    iterations = 100000;
    auto t = steady_clock::now();
    for (auto i = iterations; i > 0; i--) {
        myModel.cycleModel();
    }
    auto d = steady_clock::now() - t;
    //mytop.printTree(cout);
    cout << "Total time: " << duration_cast<nanoseconds>(d).count() << "ns"
    << ", Per iteration: " << duration_cast<nanoseconds>(d).count() / iterations << "ns"
    << endl;
    // Make a model instance and then populate it, beginning with the
    // top-most module in the design.
    SimModel*  myModel = new SimModel();
    ModuleTop* myTop = new ModuleTop(myModel);
    
    //myModel->initModel();
    myModel->resetModel();
    myModel->beginTracing("/Users/rabeles/Development/oc_test1.lxt");
    auto iterations = 600;
    for (auto i = iterations; i > 0; i--) {
        myModel->cycleModel();
    }
    myModel->endTracing();
    
    // Programatically construct a module that serves to send commands
    // and their operands to the OperatorCtl module in the design.
    // Todo: This becomes its own subclass of SimModule.
    auto opInterface = new SimModule(myModel, "oi", myTop);
    auto opCommand = new SimPortOut<int>(myModel, "command", opInterface, 0, false);
    auto opOperand = new SimPortOut<BiqWord>(myModel, "operand", opInterface, biqWordBlank, false);
    auto opSwitches = new SimPortOut<int>(myModel, "switches", opInterface, 0, false);
    auto opBusy = new SimPortIn<bool>(myModel, "busy", opInterface);
    auto wire1 = new SimWire(myModel, "oc_command", myTop,
                             "oi/command", "oc/command_in");
    auto wire2 = new SimWire(myModel, "oc_operand", myTop,
                             "oi/operand", "oc/command_op");
    auto wire3 = new SimWire(myModel, "oc_switches", myTop,
                             "oi/switches", "oc/command_sw");
    auto wire4 = new SimWire(myModel, "oi_busy", myTop,
                             "oc/busy", "oi/busy");
    SimModulePtr module = myModel->findModule("oc", myTop);
    auto opConsole = dynamic_cast<ModuleOperatorCtl *>(module);
    myModel->resetModel();
    myModel->beginTracing("/Users/rabeles/Development/oc_test2.lxt");
    opOperand->set(biqWordBlank);
    opSwitches->set(0);
    opCommand->set((int)ConsoleCommand::pressErrorResetKey);
    opConsole->doCommand();
    opCommand->set(0);
    opCommand->set((int)ConsoleCommand::pressErrorSenseResetKey);
    opConsole->doCommand();
    opCommand->set(0);
    
    opOperand->set(BiqWord("+1234"));
    opSwitches->set(0);
    opCommand->set((int)ConsoleCommand::setAddressSelectSwitches);
    opConsole->doCommand();
    opCommand->set(0);
    
    for (auto i = iterations; i > 0; i--) {
        myModel->cycleModel();
    }
    myModel->endTracing();
    auto controller = new ModelController();
    controller->beginTracing("/Users/rabeles/Development/sc_test1.lxt");
    controller->cycle();
    controller->cycleUntilNotBusy();
    controller->setDist(BiqWord("+0123456789"));
    controller->setAccLower(BiqWord("+0123456789"));
    controller->setAccUpper(BiqWord("+0123456789"));
    for (int i = 0; i < 100; i++) {
        controller->cycle();
    }
    controller->cycleUntil(ddx);
    controller->endTracing();
    controller->printConnectionReport(cout);
#endif
    OperatorTestCtl* operatorTest = new OperatorTestCtl;
    operatorTest->runTest();
    delete operatorTest;
    operatorTest = nullptr;
    
    AddrRegTestControl* addrRegTest = new AddrRegTestControl;
    addrRegTest->runTest();
    delete addrRegTest;
    addrRegTest = nullptr;
    
    AdderTestCtl* adderTest = new AdderTestCtl;
    adderTest->runTest();
    delete adderTest;
    adderTest = nullptr;
    
    ErrorStopTestCtl* errStopTest = new ErrorStopTestCtl;
    errStopTest->runTest();
    delete errStopTest;
    errStopTest = nullptr;
    
    CtlCommTestControl* ctlCommTest = new CtlCommTestControl;
    ctlCommTest->runTest();
    delete ctlCommTest;
    ctlCommTest = nullptr;
    
    return 0;
}
