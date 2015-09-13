//
//  module_timing.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

using namespace std;

ModuleTiming::ModuleTiming(SimModelPtr m, const std::string& n, SimObjPtr p)
    : SimModule(m, n, p)
{
    digitNumber  = new POutIntReg (m, "digit_num", this, 0);
    wordNumber   = new POutIntReg (m, "word_num", this, 0);
    sectorNumber = new POutIntReg (m, "sector_num", this, 0);
    earlyDigitNumber
                 = new POutIntReg (m, "e_digit_num", this, 0);
    upperWord    = new POutBoolReg(m, "upper_word", this, 0);
    lowerWord    = new POutBoolReg(m, "lower_word", this, 0);
}
vector<SimDispatchable> ModuleTiming::getDispatchables() {
    vector<SimDispatchable> dispatchables;
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dSeq, &SimModule::cycleSeqD));
    dispatchables.push_back(SimDispatchable(this, DispatchGroup::dClock, &SimModule::cycleClockD));
    return dispatchables;
}
void ModuleTiming::cycleSeqD() {
    digitNumber->set(cycleNumber % 12);
    int wn = cycleNumber / 12 % 10;
    *wordNumber = wn;
    *upperWord = wn & 1;
    *lowerWord = ~wn & 1;
    *sectorNumber = cycleNumber / (12 * 10) % 5;
    *earlyDigitNumber = (cycleNumber + 1) % 12;
    cycleNumber++;
}
void ModuleTiming::cycleClockD() {
    digitNumber->cycle();
    wordNumber->cycle();
    sectorNumber->cycle();
    earlyDigitNumber->cycle();
    upperWord->cycle();
    lowerWord->cycle();
}
void ModuleTiming::reset() {
    cycleNumber = 1;
}
