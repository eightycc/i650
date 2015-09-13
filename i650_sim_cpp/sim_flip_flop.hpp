//
//  sim_flip_flop.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/15/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_flip_flop_hpp
#define sim_flip_flop_hpp

// This is identical to SimPortOut, which of course it is because out ports are
// actually flip-flops. This can be consolodated into a single template descended
// from a SimMemoryBase object.

// Clocked -- value changes when clocked
// Not clocked -- value changes immediately
// Edge triggered -- value changes only on value != nextvalue && value == edgevalue
// Edge Value -- indicates edge detection direction (up / down)

template<typename T, Clocking C, Triggering E> class SimFlipFlop;

typedef SimFlipFlop<bool, Clocking::none, Triggering::level>        FFBool;
typedef SimFlipFlop<bool, Clocking::clocked, Triggering::level>     FFBoolReg;
typedef SimFlipFlop<int, Clocking::none, Triggering::level>         FFInt;
typedef SimFlipFlop<int, Clocking::clocked, Triggering::level>      FFIntReg;
typedef SimFlipFlop<BiqDigit, Clocking::none, Triggering::level>    FFBiq;
typedef SimFlipFlop<BiqDigit, Clocking::clocked, Triggering::level> FFBiqReg;
typedef SimFlipFlop<bool, Clocking::none, Triggering::edge>        FFBoolEdge;
typedef SimFlipFlop<bool, Clocking::clocked, Triggering::edge>     FFBoolEdgeReg;
typedef SimFlipFlop<int, Clocking::none, Triggering::edge>         FFIntEdge;
typedef SimFlipFlop<int, Clocking::clocked, Triggering::edge>      FFIntEdgeReg;
typedef SimFlipFlop<BiqDigit, Clocking::none, Triggering::edge>    FFBiqEdge;
typedef SimFlipFlop<BiqDigit, Clocking::clocked, Triggering::edge> FFBiqEdgeReg;

template<typename T, Clocking C, Triggering E>
class SimFlipFlop : public SimMemory<T, SimCategory::outPort, C, E> {
public:
    SimFlipFlop(SimModelPtr m, const std::string& n, SimObjPtr p, T i = T(), T e = T())
    : SimMemory<T, SimCategory::outPort, C, E>(m, n, p, i, e)
    { }
    virtual const T& operator =(T v) {
        this->set(v);
        return this->getNextValue();
    }
};
#endif /* sim_flip_flop_hpp */
