//
//  sim_model.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 8/2/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_model_hpp
#define sim_model_hpp

// Setting up goes like this:
// 1. Instantiate SimModel object
// 2. Instantiate top-level module, passing SimModelPtr to constructor.
//    In SimObject constructor SimModel.registerSimObject() to make
//    object known to model, model adds SimObjPtr to simObjects.
// 3. SimModel.initModel() to build category lists, dispatch lists, and connect wires
// 4. SimModel.reset() to fully reset all simulator objects

using SimDispatchCall = void (SimModule::*)();
struct SimDispatchable {
    SimModulePtr    module;
    DispatchGroup   group;
    SimDispatchCall call;
    SimDispatchable(SimModulePtr o, DispatchGroup g, SimDispatchCall c)
      : module(o)
      , group(g)
      , call(c)
    { }
};

class SimModel {
    std::vector<SimObjPtr> simObjects;  // all sim objs in model
    std::map<std::string, SimObjPtr> simObjectsByName;
    std::vector<std::vector<SimObjPtr>> categoryLists;
    std::vector<std::vector<SimDispatchable>> dispatchLists;
    std::vector<SimDispatchable> fastDispatchList;
    bool initialized       = false;
    lt_trace *traceContext = nullptr;
    long long traceTime    = 0;
    bool tracing           = false;
    lt_symbol* tracePhase  = nullptr;

    void initModel();
    void buildObjectsByName();
    void buildCategoryLists();
    void buildDispatchLists();
    void connectWires();
    void trace();
    
  public:
    SimModel();
    ~SimModel();
    void registerSimObject(SimObjPtr);
    void resetModel();
    void cycleModel();
    SimObjPtr lookupByFullName(const std::string& name);
    SimModulePtr findModule(const std::string& nm, SimObjPtr root);
    void beginTracing(const std::string& fn);
    void pauseTracing();
    void resumeTracing();
    void endTracing();
    void printConnectionReport(std::ostream& s) const;
};

#endif /* sim_model_hpp */
