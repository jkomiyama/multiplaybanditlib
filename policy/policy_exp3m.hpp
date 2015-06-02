#pragma once

#include "policy.hpp"

namespace bandit{

class Exp3MPolicy : public Policy{
  const uint K;
  const double gamma;
  std::vector<double> wi; 
  std::vector<double> pi; 
public:
  Exp3MPolicy(uint K, double gamma): K(K), gamma(gamma) {
    reset();
  }
  void reset(){
    wi = std::vector<double>(K, 1.0);
    pi = std::vector<double>(K, 1.0); //initial value of pi should not be used 
  }
  std::vector<uint> depRound(uint l, std::vector<double> ps){
    //std::cout << "depround start" << std::endl;
    if ( std::abs(vectorSum(ps)-l) > 0.001 ) {
      std::cout << "l= " << l << " wi/pi= " ;
      for(uint i=0;i<K;++i){
        std::cout << wi[i] << "/" << ps[i] << " ";
      }
      std::cout << std::endl;
 
      std::cerr << "Error: probability sum is not equal to " << l << " (" << vectorSum(ps) << ")" << std::endl;
      abort();
    }
    std::vector<int> selected(K, -1); //-1-> not determined 0->not selected 1->selected
    for(uint i=0;i<K;++i){ 
      if(ps[i]>0.999){
        selected[i] = 1;
      }else if(ps[i]<0.001){
        selected[i] = 0;
      }
    }
    std::vector<uint> randIndices = randomIndices(K);
    while(true){
      std::pair<uint, uint> ij;
      bool iSelected = false;
      bool iSelected2 = false;
      uint i;
      for(i=0;i<K;++i){ 
        //std::cout << "itemp=" << i << std::endl;
        uint ind = randIndices[i];
        if(!iSelected){
          if(selected[ind]==-1){
            ij.first = ind;
            iSelected = true;
          }
        }else{
          if(selected[ind]==-1){
            ij.second = ind;
            iSelected2 = true;
            break;
          }
        }
      }
      if( (i==K) && (iSelected==false) ){
        break;
      }
      if( i==K && (!iSelected2) ){
        if(ps[ij.first]>0.5){
          selected[ij.first]=1;
        }else{
          selected[ij.first]=0;
        }
        break;
      }
      std::pair<double, double> pij;
      pij.first  = ps[ij.first];
      pij.second = ps[ij.second];
      double alpha = std::min(1-pij.first, pij.second);
      double beta  = std::min(pij.first, 1-pij.second);
      double prob = beta/(alpha+beta);
      double rand = std::uniform_real_distribution<double>(0.0,1.0)(randomEngine);
      double pin,pjn;
      if (rand < prob){
        pin = pij.first  + alpha;
        pjn = pij.second - alpha;
      }else{
        pin = pij.first  - beta;
        pjn = pij.second + beta;
      }
      if(pin>0.999){
        selected[ij.first] = 1;
      }else if(pin<0.001){
        selected[ij.first] = 0;
      }else{
        ps[ij.first] = pin;
      }
      if(pjn>0.999){
        selected[ij.second] = 1;
      }else if(pjn<0.001){
        selected[ij.second] = 0;
      }else{
        ps[ij.second] = pjn;
      }
    }
    std::vector<uint> selectedArms;
    for(uint i=0;i<K;++i){
      if(selected[i]==1){
        selectedArms.push_back(i);
      }
    }
    if(selectedArms.size()!=l){
      std::cerr << "Error: " << selectedArms.size() << " selected (should be " << l << " )." << std::endl;
      abort();     
    }
    return selectedArms;
  }
  double getAlpha(double rhs, const std::vector<double> &wsSorted){
   double s = vectorSum(wsSorted);
    for(uint k=0;k<=K;++k){
      double alpha = (rhs * s) / (1 - k * rhs);
      double current = wsSorted[k];
      if(alpha>current){
        return alpha;
      }
      s -= current;
    }
    std::cerr << "Error: alpha not found." << std::endl;
    abort();
  }
  virtual std::vector<uint> selectNextArms(uint l){
    const double wsum = vectorSum(wi);
    std::vector<double> wsSorted(wi);
    std::sort(wsSorted.begin(), wsSorted.end(), std::greater<double>());
    double threshold = (1.0/l-gamma/K)*wsum/(1-gamma);
    std::vector<double> wsd(wi);
    if(wsSorted[0] >= threshold){
      const double alpha = getAlpha( (1.0/l-gamma/K)/(1-gamma), wsSorted);
      for(uint i=0;i<K;++i){
        wsd[i]=std::min(alpha, wi[i]);
      }
    }else{
      for(uint i=0;i<K;++i){
        wsd[i] = wi[i];
      }
    }
    const double wsdsum = vectorSum(wsd);
    for(uint i=0;i<K;++i){
      pi[i] = l * ( (1-gamma)*wsd[i]/wsdsum + gamma/K );
    }
    return depRound(l, pi);
  }
  virtual void updateState(std::vector<uint> is, std::vector<double> rs){
    const uint l = is.size();
    for(uint i=0;i<is.size();++i){
      uint k = is[i];
      double r = rs[i];
      wi[k] *= exp(l*gamma*(r/pi[k])/K);
    }
  }
  virtual std::string toString(){
    std::string str="Exp3M Policy with gamma=";
    str+=dtos(gamma);
    return str;
  }
};

} //namespace
