//
//  test_common.hpp
//  i650_sim_cpp
//
//  Created by Bob Abeles on 8/23/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef test_common_hpp
#define test_common_hpp

class CommonTestModule;

class CommonTestControl /* : public SimTestControl */ {
  protected:
    ModelController *modelControl;
    SimModelPtr      model = nullptr;
    SimModulePtr     topModule = nullptr;
    CommonTestModule      *test = nullptr;
  public:
    CommonTestControl();
    virtual ~CommonTestControl();
    virtual void initialize(SimModelPtr, SimModulePtr);
    
    bool doCommand(ConsoleCommand);
    bool doCommand(ConsoleCommand, int);
    bool doCommand(ConsoleCommand, const BiqWord&);
    void fillDrum();
};

class CommonTestModule : public SimModule {
  public:
    CommonTestModule(SimModelPtr, const std::string&, SimModulePtr);
};

#endif /* test_common_hpp */
