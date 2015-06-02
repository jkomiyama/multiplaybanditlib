#pragma once

#include "policy.hpp"

namespace bandit{

class UCBPolicy : public Policy{
  const uint K;
  const double alpha;
  std::vector<double> Ni;
  std::vector<double> Gi; 
public:
  UCBPolicy(uint K, double alpha=1.0): K(K), alpha(alpha) {
    reset();
  }
  void reset(){
    Ni = std::vector<double>(K, 0.0);
    Gi = std::vector<double>(K, 0.0);
  }
  virtual std::vector<uint> selectNextArms(uint l){
    double n = vectorSum(Ni);
    std::vector<double> indices = std::vector<double>(K, 0.0); 
    for(uint k=0;k<K;++k){
      if(Ni[k]==0){
        indices[k] = 100000000; //very large number
      }else{
        indices[k] = (Gi[k]/Ni[k]) + sqrt( (alpha * log(n)) / (double)Ni[k] );
      }
    }

    std::vector<uint> targetArms = vectorMaxIndices(indices, l);
    return targetArms;
  }
  virtual void updateState(std::vector<uint> is, std::vector<double> rs){
    for(uint i=0;i<is.size();++i){
      uint k = is[i];
      Ni[k]+=1;
      Gi[k]+=rs[i];
    }
  }
  virtual std::string toString(){
    std::string str="UCB Policy with alpha=";
    str+=dtos(alpha);
    return str;
  }
};

} //namespace
