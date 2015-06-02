#pragma once

#include "util.hpp"
#include "../arm/arm.hpp"
#include "../policy/policy.hpp"
#include "../bandit/roundwiselog.hpp"

namespace bandit{

typedef std::shared_ptr<Arm> ArmPtr;
typedef std::shared_ptr<Policy> PolicyPtr;

template<class Log = RoundwiseLog>
class Simulator{
  std::vector<ArmPtr>    arms;
  std::vector<PolicyPtr> policies;
  std::vector<uint> optimalArms;
  std::vector<bool> isOptimalArmFlag;
  const uint L, K; //selects l out of k arms in each round
public:
  Simulator(const std::vector<ArmPtr> &arms, const std::vector<PolicyPtr> &policies, int L):
    arms(arms), policies(policies), L(L), K(arms.size()) {
    std::vector<double> expectedRewards(arms.size(), 0.0);
    for(uint i=0;i<arms.size();++i){
      expectedRewards[i] = arms[i]->getExpectedReward();
    }
    optimalArms = vectorMaxIndices(expectedRewards, L);
    isOptimalArmFlag = std::vector<bool>(arms.size(), false);
    for(uint i=0;i<L;++i){
      isOptimalArmFlag[optimalArms[i]] = true;
      //std::cout << "arm " << optimalArms[i] << " is optimal" << std::endl;
    }
  }
  bool isOptimalArm(int i){
    if(i<0 || i>=K){
      std::cerr << "arm number must be between 0 and " << L-1 << std::endl;
      abort();
    }
    return isOptimalArmFlag[i];
  }
  void debugPrint(){
    for(uint i=0;i<arms.size();++i){
      std::cout << arms[i]->toString() << std::endl;
    }  
    for(uint p=0;p<policies.size();++p){
      std::cout << policies[p]->toString() << std::endl;
    }  
  }
  void run(Log &log, const uint T){
    log.startRun();
    for(uint t=1;t<=T;++t){
      for(uint p=0;p<policies.size();++p){
        execSingleRound(log, p, t);
      }
    }
  }
  void execSingleRound(Log &log, uint p, uint t){
    std::vector<uint> is = policies[p]->selectNextArms(L);
    std::vector<double> rewards;
    for(uint i=0;i<is.size();++i){
      rewards.push_back( arms[is[i]]->pull() );
    }
    policies[p]->updateState(is, rewards); 
    double optimalExpectedReward = 0;
    for(uint i=0;i<L;++i){
      optimalExpectedReward += arms[optimalArms[i]]->getExpectedReward();
    }
    double armExpectedReward = 0;
    for(uint i=0;i<L;++i){
      armExpectedReward += arms[is[i]]->getExpectedReward();
    }
    double regret = optimalExpectedReward - armExpectedReward;

    log.record(p, t, is, vectorSum(rewards), regret);
  }
};


} //namespace
