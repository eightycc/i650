//
//  sim_common.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/5/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_common_hpp
#define sim_common_hpp

class SimModel;
class SimObject;
class SimPort;
class SimWire;
class SimModule;

typedef SimModule* SimModulePtr;
typedef SimObject* SimObjPtr;
typedef SimModel*  SimModelPtr;
typedef SimWire*   SimWirePtr;

static const BiqDigit biqZero (BiqDigit::biq0);
static const BiqDigit biqBlank(BiqDigit::biqBlank);
static const BiqDigit biq0(BiqDigit::biq0);
static const BiqDigit biq1(BiqDigit::biq1);
static const BiqDigit biq2(BiqDigit::biq2);
static const BiqDigit biq3(BiqDigit::biq3);
static const BiqDigit biq4(BiqDigit::biq4);
static const BiqDigit biq5(BiqDigit::biq5);
static const BiqDigit biq6(BiqDigit::biq6);
static const BiqDigit biq7(BiqDigit::biq7);
static const BiqDigit biq8(BiqDigit::biq8);
static const BiqDigit biq9(BiqDigit::biq9);
static const BiqDigit biqPlus(BiqDigit::biqPlus);   // signs MUST use these
static const BiqDigit biqMinus(BiqDigit::biqMinus);
static const auto ddx = BiqWord::ddx;
static const auto dd0 = BiqWord::dd0;
static const auto dd1 = BiqWord::dd1;
static const auto dd2 = BiqWord::dd2;
static const auto dd3 = BiqWord::dd3;
static const auto dd4 = BiqWord::dd4;
static const auto dd5 = BiqWord::dd5;
static const auto dd6 = BiqWord::dd6;
static const auto dd7 = BiqWord::dd7;
static const auto dd8 = BiqWord::dd8;
static const auto dd9 = BiqWord::dd9;
static const auto dd10 = BiqWord::dd10;

static const BiqWord biqWordBlank;

enum class SimCategory {
    inPort = 0,
    outPort,
    module,
    wire,
    flipFlop,
    edgeDetect,
    last
};
static const std::vector<std::string> categoryNames = {
    "inPort", "outPort", "module", "wire", "flipFlop", "edgeDetect"
};

enum class DispatchGroup {
    aCombo = 0,
    aSeq,
    aClock,
    bCombo,
    bSeq,
    bClock,
    cCombo,
    cSeq,
    cClock,
    dCombo,
    dSeq,
    dClock,
    last
};

enum class Triggering {
    level, edge
};

enum class Clocking {
    none, clocked
};

//template<typename T, SimCategory P, Clocking C, Triggering E> class SimMemory;


#endif /* sim_common_hpp */
