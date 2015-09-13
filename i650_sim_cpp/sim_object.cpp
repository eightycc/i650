//
//  sim_object.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"

using namespace std;

SimObject::SimObject(SimModelPtr m, const string& n, SimObjPtr p, SimCategory c)
     : model(m)
     , rootName(n)
     , category(c)
     , parent  (p)
     , children()
{
    SimObjPtr pobj = parent;
    while (pobj) {
        if (prefixName.empty()) {
            prefixName = pobj->rootName;
        } else {
            prefixName = pobj->rootName + '/' + prefixName;
        }
        pobj = pobj->parent;
    }
    if (prefixName.empty()) {
        fullName = categoryNames[(int)category] + '/' + rootName;
    } else {
        fullName = categoryNames[(int)category] + '/' + prefixName + '/' + rootName;
    }
    if (parent) {
        parent->addChild(this);
    }
    model->registerSimObject(this);
}
void SimObject::addChild(SimObjPtr c) {
    children.push_back(c);
}
void SimObject::print(ostream& s, int level) const {
    for (int lev = level; lev > 0; lev--)
        s << "  ";
    s << "fullName: "       << fullName
      << ", rootName: "     << rootName
      << ", prefixName: "   << prefixName
      << ", categoryName: " << categoryNames[(int)category]
      << endl;
}
void SimObject::printTree(ostream& s, int d) {
    print(s, d);
    //s << endl;
    for (auto child : children) {
        child->printTree(s, d+1);
        //s << endl;
    }
}
ostream& operator<<(ostream& s, const SimObject& v) {
    v.print(s);
    return s;
}
