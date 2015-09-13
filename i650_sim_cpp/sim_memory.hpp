//
//  sim_memory.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/30/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_memory_hpp
#define sim_memory_hpp

class SimMemoryBase : public SimObject {
    int fanout = 0;
  public:
    SimMemoryBase(SimModelPtr m, const std::string& n, SimObjPtr p, SimCategory t)
      : SimObject(m, n, p, t)
    {
    }
    virtual Clocking getClocking() const = 0;
    virtual Triggering getTriggering() const = 0;
    virtual void beginTracing(lt_trace *, const std::string&) = 0;
    virtual void trace(lt_trace *) = 0;
    int getFanout() const {return fanout;}
    void incrementFanout() {fanout++;}
    virtual size_t getMemoryTypeHash() const = 0;
};

template<typename T>
class SimMemoryTemplate : public SimMemoryBase {
    T* currentValuePtr;
  public:
    SimMemoryTemplate(SimModelPtr m, const std::string& n, SimObjPtr p, SimCategory c, T* vp)
      : SimMemoryBase(m, n, p, c)
      , currentValuePtr(vp)
    { }
    T* getCurrentValuePtr() const { return currentValuePtr; }
};

template<typename T, SimCategory P, Clocking C, Triggering E>
class SimMemory : public SimMemoryTemplate<T> {
    //friend class SimPortIn<T>;
    T resetValue;
    T currentValue;
    T nextValue;
    T previousValue;
    T previousValueIn;
    T edgeValue;
    Clocking clocking;
    Triggering triggering;
    lt_symbol* traceSymbol;
    size_t memoryTypeHash;
    //std::type_info memoryTypeInfo;
  public:
    SimMemory(SimModelPtr m, const std::string& n, SimObjPtr p, T i, T e)
      : SimMemoryTemplate<T>(m, n, p, P, &currentValue)
      , resetValue(i)
      , currentValue(i)
      , nextValue(i)
      , previousValue(i)
      , previousValueIn(i)
      , edgeValue(e)
      , clocking(C)
      , triggering(E)
      , memoryTypeHash(typeid(currentValue).hash_code())
    {
    }
    virtual size_t getMemoryTypeHash() const { return memoryTypeHash; }
    // This function *should* compile to two tests and one or two assignments
    // courtesy of template expansion and constant elimination.
    void set(T v) {
        switch (E) {
            case Triggering::edge:
                switch (C) {
                    case Clocking::none:
                        if (v != previousValueIn && v == edgeValue) {
                            currentValue = nextValue = v;
                        }
                        break;
                    case Clocking::clocked:
                        if (v != previousValueIn && v == edgeValue) {
                            nextValue = v;
                        }
                        break;
                }
                previousValueIn = v;
                break;
            case Triggering::level:
                switch (C) {
                    case Clocking::none:
                        currentValue = nextValue = v;
                        break;
                    case Clocking::clocked:
                        nextValue = v;
                        break;
                }
                break;
        }
    }
    const T& operator =(T v) {
        set(v);
    }
    const T& getNextValue() const {
        return nextValue;
    }
    operator T() const {return currentValue;}
    virtual void reset() {
        currentValue = nextValue = previousValue = previousValueIn = resetValue;
    }
    virtual void cycle() {
        currentValue = nextValue;
    }
    virtual Clocking getClocking() const { return clocking; }
    virtual Triggering getTriggering() const { return triggering; }
    virtual void print(std::ostream& s, int level = 0) const {
        int indent = level + 1;
        SimObject::print(s, level);
        for (int idt = indent; idt > 0; idt--)
            s << "  ";
        s << "resetValue: " << resetValue
        << " , currentValue: " << currentValue
        << " , nextValue: " << nextValue
        << " , previousValue: " << previousValue
        << " , previousValueIn: " << previousValueIn
        << " , edgeValue: " << edgeValue
        << std::endl;
    }
    virtual void beginTracing(lt_trace *ctx, const std::string& nm) {
        previousValue = currentValue;
        std::string typeName = typeid(T).name();
        if (typeName == typeid(int).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 7, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(bool).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 0, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(BiqDigit).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 6, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(BiqWord).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 12, 6, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else {
            std::cout << "*** Unrecognized port type: " << typeName
            << " ***" << std::endl;
        }
    }
    virtual void trace(lt_trace *ctx) {
        if (currentValue != previousValue) {
            emitTraceEntry(ctx, currentValue);
            previousValue = currentValue;
        }
    }
    // Function overloading gets the right emit for the job.
    void emitTraceEntry(lt_trace* ctx, int dummy) {
        lt_emit_value_int(ctx, traceSymbol, 0, (int)currentValue);
    }
    void emitTraceEntry(lt_trace* ctx, BiqDigit dummy) {
        lt_emit_value_int(ctx, traceSymbol, 0, (int)currentValue);
    }
    void emitTraceEntry(lt_trace* ctx, BiqWord dummy) {
        for (auto i = 0; i < 12; i++) {
            lt_emit_value_int(ctx, traceSymbol, i, (int)currentValue[i]);
        }
    }
};

class SimEdgeDetectBase : public SimObject {

  public:
    SimEdgeDetectBase(SimModelPtr m, const std::string& n, SimObjPtr p)
      : SimObject(m, n, p, SimCategory::edgeDetect)
    { }
    virtual void beginTracing(lt_trace *, const std::string&) = 0;
    virtual void trace(lt_trace *) = 0;
};

template <typename T>
class SimEdgeDetect : public SimEdgeDetectBase {
    T resetValue;
    T currentValue;
    T nextValue;
    T previousValue;
    T previousValueIn;
    T edgeValue;
    lt_symbol* traceSymbol = nullptr;
  public:
    SimEdgeDetect(SimModelPtr m, const std::string& n, SimObjPtr p, T i, T e)
    : SimEdgeDetectBase(m, n, p)
    , resetValue(i)
    , currentValue(i)
    , nextValue(i)
    , previousValue(i)
    , previousValueIn(i)
    , edgeValue(e)
    { }
    
    void set(T v) {
        if (v != previousValueIn && v == edgeValue) {
            nextValue = v;
        }
        previousValueIn = v;
    }
    const T& operator =(T v) {
        set(v);
    }
    const T& getNextValue() const {
        return nextValue;
    }
    operator T() const {return currentValue;}
    virtual void reset() {
        currentValue = nextValue = previousValue = previousValueIn = resetValue;
    }
    virtual void cycle() {
        currentValue = nextValue;
    }
    virtual void print(std::ostream& s, int level = 0) const {
        int indent = level + 1;
        SimObject::print(s, level);
        for (int idt = indent; idt > 0; idt--)
            s << "  ";
        s << "resetValue: " << resetValue
        << " , currentValue: " << currentValue
        << " , nextValue: " << nextValue
        << " , previousValue: " << previousValue
        << " , previousValueIn: " << previousValueIn
        << " , edgeValue: " << edgeValue
        << std::endl;
    }
    virtual void beginTracing(lt_trace *ctx, const std::string& nm) {
        previousValue = currentValue;
        std::string typeName = typeid(T).name();
        if (typeName == typeid(int).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 7, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(bool).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 0, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(BiqDigit).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 0, 6, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else if (typeName == typeid(BiqWord).name()) {
            traceSymbol = lt_symbol_add(ctx, nm.c_str(), 12, 6, 0, LT_SYM_F_BITS);
            emitTraceEntry(ctx, currentValue);
        } else {
            std::cout << "*** Unrecognized edge detect type: " << typeName
            << " ***" << std::endl;
        }
    }
    virtual void trace(lt_trace *ctx) {
        if (currentValue != previousValue) {
            emitTraceEntry(ctx, currentValue);
            previousValue = currentValue;
        }
    }
    // Function overloading gets the right emit for the job.
    void emitTraceEntry(lt_trace* ctx, int dummy) {
        lt_emit_value_int(ctx, traceSymbol, 0, (int)currentValue);
    }
    void emitTraceEntry(lt_trace* ctx, BiqDigit dummy) {
        lt_emit_value_int(ctx, traceSymbol, 0, (int)currentValue);
    }
    void emitTraceEntry(lt_trace* ctx, BiqWord dummy) {
        for (auto i = 0; i < 12; i++) {
            lt_emit_value_int(ctx, traceSymbol, i, (int)currentValue[i]);
        }
    }
};

#endif /* sim_memory_hpp */
