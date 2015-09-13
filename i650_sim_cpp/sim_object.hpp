//
//  sim_object.hpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/24/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#ifndef sim_object_hpp
#define sim_object_hpp

class SimObject {
    SimModelPtr model;
    std::string fullName;
    std::string rootName;
    std::string prefixName;
    SimCategory category;
    SimObjPtr   parent;
    std::vector<SimObjPtr> children;
    void addChild(SimObjPtr c);

  public:
    SimObject(SimModelPtr m, const std::string& n, SimObjPtr p, SimCategory c);
    virtual void print(std::ostream& s, int level = 0) const;
    const std::string& getFullName() const {return fullName;}
    const std::string& getRootName() const {return rootName;}
    const std::string& getPrefixName() const {return prefixName;}
    SimCategory getCategory() const {return category;}
    SimObjPtr getParent() const {return parent;}
    const std::vector<SimObjPtr>& getChildren() const {return children;}
    SimModelPtr getModel() const {return model;}
    void printTree(std::ostream &, int = 0);
    
    virtual void reset() {}
    virtual void cycle() {}
};

std::ostream& operator<<(std::ostream& s, const SimObject& v);
#endif /* sim_object_hpp */
