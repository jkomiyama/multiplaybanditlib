#pragma once

#include "policy.hpp"

namespace bandit{

//Thompson sampling (binary reward and beta prior)
class ThompsonBinaryPolicy : public Policy{
  const uint K;
  std::vector<double> alphas, betas;
  const bool basic;
public:
  ThompsonBinaryPolicy(uint K, bool basic=true, double alpha=1, double beta=1): K(K), basic(basic) {
    for(uint i=0;i<K;++i){
      alphas.push_back(alpha);
      betas.push_back(beta);
    }
  }
  virtual std::vector<uint> selectNextArms(uint l){
    if(basic){
      std::vector<double> thetas(K, 0.0);
      for(uint i=0;i<K;++i){
        thetas[i] = beta_distribution<double>(alphas[i], betas[i])(randomEngine) ;
      }
      std::vector<uint> thetaIndices = vectorMaxIndices(thetas, l);
 
      return thetaIndices;
    }else{
      std::vector<double> means(K, 0.0);
      for(uint i=0;i<K;++i){
        means[i] = alphas[i] / betas[i];
      }
      std::vector<uint> meanIndices = vectorMaxIndices(means, l); 
      std::map<uint, bool> meanIndicesMap;
      for(uint i=0;i<l-1;++i){ //l-1 <- notice
        meanIndicesMap[meanIndices[i]] = true;
      }
      
      std::vector<double> thetas(K, 0.0);
      for(uint i=0;i<K;++i){
        thetas[i] = beta_distribution<double>(alphas[i], betas[i])(randomEngine);
      }
      std::vector<uint> thetaIndices = vectorMaxIndices(thetas, l); 
      for(uint i=0;i<l;++i){
        uint index = thetaIndices[i];
        if(meanIndicesMap.find(index) == meanIndicesMap.end()){
          meanIndices[l-1] = index;
          break;
        }
      }

      return meanIndices;
    }
  }
  virtual void updateState(std::vector<uint> is, std::vector<double> rs){
    for(uint i=0;i<is.size();++i){
      uint k = is[i];
      double r = rs[i];
      if(r>0.5){
        alphas[k]+=1;
      }else{
        betas[k] +=1;
      }
    }
  }
  virtual std::string toString(){
    std::string str;
    if(basic){
      str = "TS(basic)";
    }else{
      str = "TS(extended)";
    }
    return str;
  }
};

} //namespace
