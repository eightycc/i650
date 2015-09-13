//
//  test_adder.cpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/21/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"
#include "test_common.hpp"
#include "test_adder.hpp"

using namespace std;

AdderTestCtl::AdderTestCtl() {
    modelControl = new ModelController(this);
}

void AdderTestCtl::initialize(SimModelPtr m, SimModulePtr top) {
    CommonTestControl::initialize(m, top); // mandatory invokation of common test initialization
    test = new TestAdderCtl(m, "octest", top);
    new SimWire(m, "octest_acc_true_add", top, "octest/acc_true_add", "adda/acc_true_add", true);
    new SimWire(m, "octest_acc_comp_add", top, "octest/acc_comp_add", "adda/acc_comp_add", true);
    new SimWire(m, "octest_dist_true_add", top, "octest/dist_true_add", "addb/dist_true_add", true);
    new SimWire(m, "octest_dist_comp_add", top, "octest/dist_comp_add", "addb/dist_comp_add", true);
    new SimWire(m, "octest_upper_lower", top, "octest/upper_lower", "addb/upper_lower", true);
}

void AdderTestCtl::runTest() {
    cout << "///// Starting addrtst1 /////" << endl;
    auto addTest = dynamic_cast<TestAdderCtl *>(test);
    modelControl->beginTracing("/Users/rabeles/Development/addrtst1.lxt");
    modelControl->cycle();
    modelControl->cycleUntilNotBusy();
    
    modelControl->setDist(BiqWord("+0123456789"));
    modelControl->setAccLower(BiqWord("+0123456789"));
    modelControl->setAccUpper(BiqWord("+0123456789"));
    modelControl->cycleUntil(false, dd0); // ed1l
    *(addTest->accTrueAdd) = true;
    *(addTest->distTrueAdd) = true;
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    *(addTest->accTrueAdd) = false;
    *(addTest->distTrueAdd) = false;
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    // cause carry from low word to high word
    modelControl->setDist(BiqWord("+1"));
    modelControl->setAccLower(BiqWord("+9999999999"));
    modelControl->setAccUpper(BiqWord("+0"));
    modelControl->cycleUntil(false, dd0); // ed1l
    *(addTest->accTrueAdd) = true;
    *(addTest->distTrueAdd) = true;
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    *(addTest->accTrueAdd) = false;
    *(addTest->distTrueAdd) = false;
    modelControl->cycle();
    modelControl->cycleUntil(false, ddx);
    
    
    modelControl->endTracing();
    //modelControl->printConnectionReport(cout);
    cout << "///// Ending addrtst1 /////" << endl;
}

TestAdderCtl::TestAdderCtl(SimModelPtr m, const string& nm, SimModulePtr p)
: CommonTestModule(m, nm, p)
{
    accTrueAdd = new POutBool(m, "acc_true_add", this, false);
    accCompAdd = new POutBool(m, "acc_comp_add", this, false);
    distTrueAdd = new POutBool(m, "dist_true_add", this, false);
    distCompAdd = new POutBool(m, "dist_comp_add", this, false);
    upperLower = new POutBool(m, "upper_lower", this, false);
}