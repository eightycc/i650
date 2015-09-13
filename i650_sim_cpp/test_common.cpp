//
//  test_common.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"

using namespace std;

CommonTestControl::CommonTestControl() {
    // modelControl initialized by subclass to preserve full type of *this.
}
CommonTestControl::~CommonTestControl() {
    delete modelControl;
}

void CommonTestControl::initialize(SimModelPtr m, SimModulePtr top) {
    model = m;
    topModule = top;
}

void CommonTestControl::fillDrum() {
    // fill drum with pattern based on word address
    for (int addr = 0; addr < 2000; addr++) {
        long long data = addr + (addr * 10000LL) + ((addr % 100) * 100000000LL);
        modelControl->setDrum(addr, BiqWord(data));
    }
}

bool CommonTestControl::doCommand(ConsoleCommand c) {
    if (!modelControl->doCommand(c)) {
        cout << "*** Press key, command: " << (int)c << " initiation failed. ***" << endl;
        return false;
    };
    modelControl->cycle();
    modelControl->cycleUntil(ddx);
    return true;
}
bool CommonTestControl::doCommand(ConsoleCommand c, int v) {
    if (!modelControl->doCommand(c, v)) {
        cout << "*** Set switch, command: " << (int)c
        << ", operand: " << v << " initiation failed. ***" << endl;
        return false;
    };
    modelControl->cycle();
    modelControl->cycleUntil(ddx);
    return true;
}
bool CommonTestControl::doCommand(ConsoleCommand c, const BiqWord& v) {
    if (!modelControl->doCommand(c, v)) {
        cout << "*** Set switches, command: " << (int)c
        << ", operand: " << v << " initiation failed. ***" << endl;
        return false;
    };
    modelControl->cycle();
    modelControl->cycleUntil(ddx);
    return true;
}

CommonTestModule::CommonTestModule(SimModelPtr m, const string& nm, SimModulePtr p)
  : SimModule(m, nm, p)
{
}