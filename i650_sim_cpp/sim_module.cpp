//
//  sim_module.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"

using namespace std;

SimModule::SimModule(SimModelPtr m, const string& n, SimObjPtr p)
: SimObject(m, n, p, SimCategory::module)
{ }
