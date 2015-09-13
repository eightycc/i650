//
//  sim_ports.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/26/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"

using namespace std;

SimWire::SimWire(SimModelPtr m, const string& n, SimObjPtr p, const string& source, const string& sink, bool f)
  : SimObject (m, n, p, SimCategory::wire)
  , forceForTest(f)
{
    string prefixInPort = categoryNames[(int)SimCategory::inPort];
    string prefixOutPort = categoryNames[(int)SimCategory::outPort];
    if (p) {
        string prefix = p->getPrefixName();
        string root = p->getRootName();
        if (prefix.empty()) {
            sourceFullName = prefixOutPort + '/' + root + '/' + source;
            sinkFullName   = prefixInPort  + '/' + root + '/' + sink;
        } else {
            sourceFullName = prefixOutPort + '/' + root + '/' + source;
            sinkFullName   = prefixInPort  + '/' + root + '/' + sink;
        }
    } else {
        sourceFullName = prefixOutPort + '/' + source;
        sinkFullName   = prefixInPort  + '/' + sink;
    }
}
void SimWire::connect() {
    if (connected) {
        cout << "*** Wire already connected: "
             << getFullName() << " ***" << endl;
        return;
    }
    SimObjPtr sourceObj = getModel()->lookupByFullName(sourceFullName);
    SimObjPtr sinkObj   = getModel()->lookupByFullName(sinkFullName);
    if (!sourceObj) {
        cout << "*** Unable to find connection source: "
             << sourceFullName
             << ", fullName: " << getFullName() << " ***"
             << endl;
        return;
    }
    if (!sinkObj) {
        cout << "*** Unable to find connection sink: "
             << sinkFullName
             << ", fullName: " << getFullName() << " ***"
             << endl;
        return;
    }
    //cout << "+++ connect() sourceObj: " << sourceObj->getFullName()
    //     << ", sinkObj: " << sinkObj->getFullName()
    //     << endl;
    auto portIn = dynamic_cast<SimPortInBase *>(sinkObj);
    portIn->connect(sourceObj, forceForTest);
    connected = true;
}
void SimWire::print(ostream& s, int level) const {
    int indent = level;
    SimObject::print(s, level);
    for (int idt = indent; idt > 0; idt--)
        s << "  ";
    s << "sourceFullName: " << sourceFullName
      << " , sinkFullName: " << sinkFullName
      << endl;
}
void SimWire::reset() {
    connected = false;
}
