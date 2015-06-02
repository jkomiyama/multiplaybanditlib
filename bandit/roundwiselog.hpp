#pragma once

#include "util.hpp"
#include "../arm/arm.hpp"
#include "../policy/policy.hpp"

namespace bandit{

class RoundwiseLog{
public:
  uint K, P, T, run;
  std::vector<std::vector<double> > roundwiseRewards;
  std::vector<std::vector<double> > roundwiseRegrets;
  RoundwiseLog(uint K, uint P, uint T): K(K), P(P), T(T) {
    roundwiseRewards = std::vector<std::vector<double> >(P, std::vector<double>(T, 0.0));
    roundwiseRegrets = std::vector<std::vector<double> >(P, std::vector<double>(T, 0.0));
    run=0;
  }
  //start new run 
  void startRun(){
    run+=1;
  }
  //policy p at round t chose arm k and received reward r
  void record(uint p, uint t, std::vector<uint> &is, double r, double regretDelta){
    roundwiseRewards[p][t]    +=r;
    roundwiseRegrets[p][t]    +=regretDelta;
  }
};

class RoundwiseLogWriter{
public:
  static void logWrite(RoundwiseLog &log,
       std::vector<std::string> armNames, std::vector<double> armExpectations, std::vector<std::string> policyNames,
       uint T, uint L, std::string filename, bool rewardPlot=true, bool regretPlot=true){
    const uint K = armNames.size();
    const uint P = policyNames.size();
    std::ofstream ofs( filename );
    ofs << "#averaged result over " << log.run << " trials" << std::endl;
    for(uint i=0;i<K;++i){
      ofs << "#arm" << i << " " << armNames[i] << std::endl;
    }
    for(uint p=0;p<P;++p){
      ofs << "#policy " << p << " " << policyNames[p] << std::endl;
    }
    ofs.setf(std::ios::fixed, std::ios::floatfield);

    std::vector<uint> optimalArms = vectorMaxIndices(armExpectations, L);
    std::vector<bool> isOptimalArmFlag = std::vector<bool>(K, false);
    for(uint i=0;i<L;++i){
      isOptimalArmFlag[optimalArms[i]] = true;
    }
    double complexity1 = 0, complexity2 = 0;
    for(uint i=0;i<K;++i){
      if(!isOptimalArmFlag[i]){ //suboptimal
        double Delta1 = armExpectations[optimalArms[L-1]] - armExpectations[i];
        double diff = kl(armExpectations[i], armExpectations[optimalArms[L-1]]);
        if(diff > 0.00001){
          complexity1 += Delta1 / diff;
          complexity2 += Delta1 / diff;
        }
        for(uint j=0;j+1<L;++j){
          double Delta2 = armExpectations[optimalArms[j]] - armExpectations[optimalArms[j+1]];
          //std::cout << "Delta2 " << j << " " << Delta2 << std::endl;
          double diff2 = kl(armExpectations[i], armExpectations[optimalArms[j]]);
          if(Delta2 > 0.00001 && diff2 > 0.00001){
            complexity2 += Delta2 / diff2;
          }
        }
      }
    }

    ofs << "#results:" << std::endl;
    ofs << "#T" ;
    ofs << " theoretical1" ;
    ofs << " theoretical2" ;
    for(uint p=0;p<P;++p){
      if(rewardPlot){
        ofs << " reward" << p;
      }
      if(regretPlot){
        ofs << " regret" << p ;
      }
    }
    ofs << std::endl;
    std::vector<double> cumulatedRewards(P, 0.0);
    std::vector<double> cumulatedRegrets(P, 0.0);
    for(uint t=0;t<T;++t){
      ofs << (t+1);
      ofs << " " << std::setprecision(2) << (complexity1 * std::log((double)t+1));
      ofs << " " << std::setprecision(2) << (complexity2 * std::log((double)t+1));
      for(uint p=0;p<P;++p){
        cumulatedRewards[p]+=log.roundwiseRewards[p][t];
        cumulatedRegrets[p]+=log.roundwiseRegrets[p][t];
        if(rewardPlot){
          ofs << std::setprecision(2) << " " << cumulatedRewards[p]/log.run;
        }
        if(regretPlot){
          ofs << std::setprecision(2) << " " << cumulatedRegrets[p]/log.run ;
        }
      }
      ofs << std::endl;
    }
  }
};

} //namespace
