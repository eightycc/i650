//
//  sim_ports.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/26/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_ports_hpp
#define sim_ports_hpp

template<typename T, Clocking C, Triggering E> class SimPortOut;
template<typename T> class SimPortIn;
template<typename T> class SimPortInM;

typedef SimPortOut<bool, Clocking::clocked, Triggering::level> POutBoolReg;
typedef SimPortOut<bool, Clocking::none, Triggering::level> POutBool;
typedef SimPortOut<int, Clocking::clocked, Triggering::level> POutIntReg;
typedef SimPortOut<int, Clocking::none, Triggering::level> POutInt;
typedef SimPortOut<BiqDigit, Clocking::clocked, Triggering::level> POutBiqReg;
typedef SimPortOut<BiqDigit, Clocking::none, Triggering::level> POutBiq;
typedef SimPortOut<BiqWord, Clocking::clocked, Triggering::level> POutBiqWordReg;
typedef SimPortOut<BiqWord, Clocking::none, Triggering::level> POutBiqWord;

typedef SimPortIn<bool>      PortInBool;
typedef SimPortIn<int>       PortInInt;
typedef SimPortIn<BiqDigit>  PortInBiq;
typedef SimPortInM<bool>     PortInBoolM;
typedef SimPortInM<int>      PortInIntM;
typedef SimPortInM<BiqDigit> PortInBiqM;
typedef SimPortIn<BiqWord>   PortInBiqWord;

class SimPortInBase : public SimObject {
    bool connected = false;
 public:
    SimPortInBase(SimModelPtr m, const std::string& n, SimObjPtr p)
      : SimObject(m, n, p, SimCategory::inPort)
    { }
    virtual void connect(SimObjPtr, bool) {}
    bool isConnected() const {return connected;}
    void markConnected() {connected = true;}
};

template<typename T>
class SimPortIn : public SimPortInBase {
    SimObjPtr sourceObj = nullptr;
    T* sourceValue;
    T  dummyValue = T();
    
  public:
    SimPortIn(SimModelPtr m, const std::string& n, SimObjPtr p)
      : SimPortInBase(m, n, p)
    {
        sourceValue = &dummyValue;
    }
    
    virtual void connect(SimObjPtr s, bool force) {
        if (isConnected() && !force) {
            std::cout << "*** Sink port already connected, "
                      << "source: " << s->getFullName()
                      << ", sink: " << getFullName()
                      << " ***" << std::endl;
            return;
        }
        auto portOut = dynamic_cast<SimMemoryTemplate<T> *>(s);
        if (typeid(dummyValue).hash_code() == portOut->getMemoryTypeHash()) {
            sourceObj = s;
            sourceValue = portOut->getCurrentValuePtr(); // <-- The connection
            markConnected();
            portOut->incrementFanout();
        } else {
            std::cout << "*** Source/sink port value type mismatch, "
                << "source: " << s->getFullName()
                << ", sink: " << getFullName()
                << " ***" << std::endl;
        }
    }
    virtual void print(std::ostream& s, int level = 0) const {
        int indent = level + 1;
        SimObject::print(s, level);
        for (int idt = indent; idt > 0; idt--)
            s << "  ";
        if (sourceObj) {
            s << " , sourceObj: " << sourceObj->getFullName()
              << " , sourceValue: " << *sourceValue;
        }
        s << std::endl;
    }
    operator T() const {return *sourceValue;}
};

template<typename T>
class SimPortInM : public SimPortInBase {
    struct SourcePort {
        SimObjPtr port;
        T*        value;
        SourcePort(SimObjPtr p, T v) : port(p), value(v) { }
    };
    
    T nullValue;
    std::vector<SourcePort> sourcePorts;
    
public:
    SimPortInM(SimModelPtr m, const std::string& n, SimObjPtr p)
      : SimPortInBase(m, n, p)
      , nullValue()
      , sourcePorts()
    {
    }
    
    virtual void connect(SimObjPtr s, bool) {
        auto portOut = dynamic_cast<SimMemoryTemplate<T> *>(s);
        if (type_info(nullValue) == portOut->getMemoryTypeInfo()) {
            sourcePorts.push_back(new SourcePort(portOut, portOut->getCurrentValuePtr()));
            portOut->incrementFanOut();
        } else {
            std::cout << "*** Source/sink port value type mismatch, "
            << "source: " << s->getFullName()
            << ", sink: " << getFullName()
            << " ***" << std::endl;
        }
    }
    virtual void print(std::ostream& s, int level = 0) const {
        int indent = level + 1;
        SimObject::print(s, level);
        for (auto source : sourcePorts) {
            for (int idt = indent; idt > 0; idt--)
                s << "  ";
            s << "sourcePort: " << source.port->getFullName()
            << " , sourceValue: " << source.value << std::endl;
        }
    }
    operator T() const {
        T value = nullValue;
        for (auto source : sourcePorts) {
            value |= source;
        }
        return value;
    }
};

template<typename T, Clocking C, Triggering E>
class SimPortOut : public SimMemory<T, SimCategory::outPort, C, E> {
  public:
    SimPortOut(SimModelPtr m, const std::string& n, SimObjPtr p, T i = T(), T e = T())
      : SimMemory<T, SimCategory::outPort, C, E>(m, n, p, i, e)
    { }
    virtual const T& operator =(T v) {
        this->set(v);
        return this->getNextValue();
    }
};

class SimWire : public SimObject {
    std::string sourceFullName;
    std::string sinkFullName;
    bool forceForTest;
    bool connected = false;
  public:
    SimWire(SimModelPtr m, const std::string& n, SimObjPtr p, const std::string& source, const std::string& sink, bool f = false);
    virtual void connect();
    virtual void reset();
    virtual void print(std::ostream& s, int level = 0) const;
    bool forced() const {return forceForTest;}
};
#endif /* sim_ports_hpp */
