#pragma once

#include "policy.hpp"

namespace bandit{

class RandomPolicy : public Policy{
  const uint K;
public:
  RandomPolicy(uint K): K(K) {
  }
  virtual std::vector<uint> selectNextArms(uint l){
    std::map<uint, bool> armsMap;
    while(armsMap.size() < l ){
      armsMap[ std::uniform_int_distribution<int>(0, K-1)(randomEngine) ] = true;
    }
    std::vector<uint> arms;
    for(std::map<uint, bool>::iterator it=armsMap.begin();it!=armsMap.end();++it){
      arms.push_back(it->first);
    }
    return arms;
  }
  virtual void updateState(std::vector<uint>, std::vector<double>){
  }
  virtual std::string toString(){
    std::string str="Random";
    return str;
  }
};

} //namespace
