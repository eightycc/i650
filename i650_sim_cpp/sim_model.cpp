//
//  sim_model.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 8/2/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"

using namespace std;

SimModel::SimModel()
{
    categoryLists.resize((int)SimCategory::last, vector<SimObjPtr>());
    dispatchLists.resize((int)DispatchGroup::last, vector<SimDispatchable>());
    fastDispatchList.clear();
}
SimModel::~SimModel() {
    endTracing();
    for (auto obj : simObjects) {
        delete obj;
    }
}
void SimModel::buildObjectsByName() {
    for (auto obj : simObjects) {
        if (simObjectsByName.find(obj->getFullName()) != simObjectsByName.end()) {
            cout << "*** Duplicate object name: " << obj->getFullName() << " ***" << endl;
        } else {
            simObjectsByName[obj->getFullName()] = obj;
        }
    }
}
void SimModel::buildCategoryLists() {
    for (auto obj : simObjects) {
        auto list = &categoryLists[(int)obj->getCategory()];
        list->push_back(obj);
    }
}
void SimModel::buildDispatchLists() {
    for (auto obj : categoryLists[(int)SimCategory::module]) {
        auto module = dynamic_cast<SimModulePtr>(obj);
        auto dispatchables = module->getDispatchables();
        for (auto dispatchable : dispatchables) {
            dispatchLists[(int)dispatchable.group].push_back(dispatchable);
        }
    }
    // Run combinatorial logic a second time to effect mux->mux propagation
    // This is very rough. A better approach will optimize based on exploring
    // signal paths through combo logic. An even better approach will combine
    // path exploration with conversion of combo logic to table lookup.
    for (auto obj : categoryLists[(int)SimCategory::module]) {
        auto module = dynamic_cast<SimModulePtr>(obj);
        auto dispatchables = module->getDispatchables();
        for (auto dispatchable : dispatchables) {
            if (dispatchable.group == DispatchGroup::aCombo
                || dispatchable.group == DispatchGroup::bCombo
                || dispatchable.group == DispatchGroup::cCombo
                || dispatchable.group == DispatchGroup::dCombo) {
                dispatchLists[(int)dispatchable.group].push_back(dispatchable);
            }
        }
    }
    for (auto list : dispatchLists) {
        for (auto dispatchable : list) {
            fastDispatchList.push_back(dispatchable);
        }
    }
}
void SimModel::connectWires() {
    for (auto obj : categoryLists[(int)SimCategory::wire]) {
            (dynamic_cast<SimWirePtr>(obj))->connect();
    }
}
void SimModel::registerSimObject(SimObjPtr obj) {
    simObjects.push_back(obj);
}
void SimModel::initModel() {
    if (initialized) {
        cout << "*** Model already initialized ***" << endl;
        return;
    }
    buildObjectsByName();
    buildCategoryLists();
    buildDispatchLists();
    connectWires();
    initialized = true;
}
void SimModel::resetModel() {
    simObjectsByName.clear();
    categoryLists.clear();
    categoryLists.resize((int)SimCategory::last, vector<SimObjPtr>());
    dispatchLists.clear();
    dispatchLists.resize((int)DispatchGroup::last, vector<SimDispatchable>());
    fastDispatchList.clear();
    initialized = false;
    for (auto obj : simObjects) {
        obj->reset();
    }
    initModel();
}
void SimModel::cycleModel() {
    if (tracing) {
        int listNumber = 0;
        int phase = 0;
        for (auto list : dispatchLists) {
            for (auto dispatchable : list) {
                (dispatchable.module->*dispatchable.call)();
            }
            if (0 == listNumber % 3) {
                lt_emit_value_int(traceContext, tracePhase, 0, phase++);
                trace();
            }
            listNumber++;
        }
    } else {
        for (auto dispatchable : fastDispatchList) {
            (dispatchable.module->*dispatchable.call)();
        }
    }
}
void SimModel::trace() {
    for (auto obj : categoryLists[(int)SimCategory::outPort]) {
        auto port = dynamic_cast<SimMemoryBase *>(obj);
        port->trace(traceContext);
    }
    for (auto obj : categoryLists[(int)SimCategory::flipFlop]) {
        auto ff = dynamic_cast<SimMemoryBase *>(obj);
        ff->trace(traceContext);
    }
    for (auto obj : categoryLists[(int)SimCategory::edgeDetect]) {
        auto ff = dynamic_cast<SimEdgeDetectBase *>(obj);
        ff->trace(traceContext);
    }
    lt_inc_time_by_delta(traceContext, 2);
}
void SimModel::beginTracing(const string& fn) {
    if (traceContext) {
        cout << "*** Trace context already exists ***" << endl;
        return;
    }
    traceContext = lt_init(fn.c_str());
    traceTime = 0;
    lt_set_timescale(traceContext, -6); // 1us timescale
    lt_set_initial_value(traceContext, 'X');
    lt_set_time64(traceContext, traceTime);
    for (auto obj : categoryLists[(int)SimCategory::outPort]) {
        auto port = dynamic_cast<SimMemoryBase *>(obj);
        if (port->getParent()) {
            string traceSignalName =  port->getParent()->getRootName() + '/'
                                    + port->getRootName();
            port->beginTracing(traceContext, traceSignalName);
        } else {
            cout << "*** Port without parent: " << port->getFullName()
                 << " ***" << endl;
        }
    }
    // add flip-flops to tracing
    for (auto obj : categoryLists[(int)SimCategory::flipFlop]) {
        auto ff = dynamic_cast<SimMemoryBase *>(obj);
        if (ff->getParent()) {
            string traceSignalName =  ff->getParent()->getRootName() + '/'
            + ff->getRootName();
            ff->beginTracing(traceContext, traceSignalName);
        } else {
            cout << "*** Flip-flop without parent: " << ff->getFullName()
            << " ***" << endl;
        }
    }
    // add edge detectors to tracing
    for (auto obj : categoryLists[(int)SimCategory::edgeDetect]) {
        auto ff = dynamic_cast<SimEdgeDetectBase *>(obj);
        if (ff->getParent()) {
            string traceSignalName =  ff->getParent()->getRootName() + '/'
            + ff->getRootName();
            ff->beginTracing(traceContext, traceSignalName);
        } else {
            cout << "*** Flip-flop without parent: " << ff->getFullName()
            << " ***" << endl;
        }
    }
    tracePhase = lt_symbol_add(traceContext, "clock_phase", 0, 1, 0, LT_SYM_F_BITS);
    lt_emit_value_int(traceContext, tracePhase, 0, 0);
    tracing = true;
}
void SimModel::endTracing() {
    if (traceContext) {
        lt_close(traceContext);
        traceContext = nullptr;
        tracing = false;
    }
}
void SimModel::pauseTracing() {
    if (!tracing) {
        cout << "*** Tracing already paused or uninitialized ***" << endl;
    } else {
        tracing = false;
    }
}
void SimModel::resumeTracing() {
    if (tracing) {
        cout << "*** Tracing already initialized and running ***" << endl;
    } else if (!traceContext) {
        cout << "*** No trace context to resume ***" << endl;
    } else {
        tracing = true;
    }
}
SimObjPtr SimModel::lookupByFullName(const string& name) {
    auto nameIter = simObjectsByName.find(name);
    return (nameIter == simObjectsByName.end())? nullptr : simObjectsByName[name];
}
SimModulePtr SimModel::findModule(const string& nm, SimObjPtr pfx) {
    string moduleName;
    SimModulePtr module = nullptr;
    if (pfx->getPrefixName().empty()) {
        moduleName = categoryNames[(int)SimCategory::module]
                       + '/' + pfx->getRootName() + '/' + nm;
    } else {
        moduleName = categoryNames[(int)SimCategory::module]
                       + '/' + pfx->getPrefixName()
                       + '/' + pfx->getRootName() + '/' + nm;
    }
    SimObjPtr obj = lookupByFullName(moduleName);
    if (obj) {
        module = dynamic_cast<SimModulePtr>(obj);
    }
    return module;
}
void SimModel::printConnectionReport(ostream &s) const {
    // Ports by module, highlighting disconnected ports
    for (auto obj : categoryLists[(int)SimCategory::module]) {
        auto module = dynamic_cast<SimModulePtr>(obj);
        s << endl << "Module: " << module->getRootName() << endl;
        for (auto childObj : module->getChildren()) {
            switch (childObj->getCategory()) {
                case SimCategory::inPort: {
                    auto port = dynamic_cast<SimPortInBase *>(childObj);
                    s << "<<-- " << port->getRootName();
                    if (port->isConnected()) {
                        s << endl;
                    } else {
                        s << " *** NOT CONNECTED ***" << endl;
                    }
                    break;
                }
                case SimCategory::outPort: {
                    auto port = dynamic_cast<SimMemoryBase *>(childObj);
                    s << "-->>" << port->getRootName()
                    << ": fanout: " << port->getFanout() << endl;
                    break;
                }
                default:
                    break;
            }
        }
    }
}
