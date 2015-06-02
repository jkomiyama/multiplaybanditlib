#include <iostream>

#include "bandit/util.hpp"
#include "arm/arm.hpp"
#include "arm/arm_bernoulli.hpp"
#include "policy/policy.hpp"
#include "policy/policy_klucb.hpp"
#include "policy/policy_ts.hpp"
#include "policy/policy_ucb.hpp"
#include "policy/policy_exp3m.hpp"
#include "policy/policy_random.hpp"
#include "bandit/simulator.hpp"
#include "cmdline.h"

using namespace std;
using namespace bandit;


vector<double> createArmExpectations(int mode, std::string filename){
  vector<double> armExpectations;
  if(mode==1){
    armExpectations.push_back(0.7);
    armExpectations.push_back(0.6);
    armExpectations.push_back(0.5);
    armExpectations.push_back(0.4);
    armExpectations.push_back(0.3);
    std::cout << "fixed arms :" << armExpectations.size() << "-armed bandits" << std::endl;
  }else if(mode==2){
    armExpectations.push_back(0.15);
    armExpectations.push_back(0.12);
    armExpectations.push_back(0.10);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.05);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    armExpectations.push_back(0.03);
    std::cout << "fixed arms :" << armExpectations.size() << "-armed bandits" << std::endl;
  }else{
    std::cout << "Error: unknown mode" << std::endl;
  }
  return armExpectations;
}

void example1(const uint T, const uint RunNum, uint mode, string filename, string outfile){
  const uint P = 7;  //=policies.size()
  uint L = 2;  //multiple play
  if(mode==2){
    L = 3;
  }
  std::cout << "L=" << L << std::endl;
  vector<ArmPtr> arms;
  vector<PolicyPtr> policies;
  vector<double> armExpectations;

  armExpectations = createArmExpectations(mode, filename);

  const uint K = armExpectations.size();
  RoundwiseLog log(K, P, T) ;
  cout << "Simulating " << RunNum << " runs." << endl;
  for(uint run=0;run<RunNum;++run){
    cout << "run=" << run << endl;
    arms.clear();
    armExpectations = createArmExpectations(mode, filename);

    for(uint i=0;i<K;++i){
      arms.push_back( ArmPtr(new BernoulliArm(armExpectations[i])) );
    }
    if(K != arms.size()){
      cout << "arm size does not match" << endl;
      abort();
    }
    policies.clear();
    policies.push_back( PolicyPtr(new ThompsonBinaryPolicy(K, true)) );
    policies.push_back( PolicyPtr(new ThompsonBinaryPolicy(K, false)) );
    policies.push_back( PolicyPtr(new KLUCBPolicy(K, true)) );
    policies.push_back( PolicyPtr(new KLUCBPolicy(K, false)) );
    policies.push_back( PolicyPtr(new UCBPolicy(K, 1.5)) );
    double exp3mGamma = std::min(1.0, std::sqrt(K*std::log(K/L)/((e-1)*L*T)));
    policies.push_back( PolicyPtr(new Exp3MPolicy(K, exp3mGamma)) );
    policies.push_back( PolicyPtr(new RandomPolicy(K)) );
    if(P != policies.size()){
      cout << "policy size does not match" << endl;
      abort();
    }
    Simulator<RoundwiseLog> sim(arms, policies, L);
    sim.run(log, T);
  }

  vector<string> armNames;
  for(uint i=0;i<arms.size();++i){
    armNames.push_back( arms[i]->toString() );
  }  
  vector<string> policyNames;
  for(uint p=0;p<policies.size();++p){
    policyNames.push_back( policies[p]->toString() );
  }
  std::cout << "output written in out/" << outfile << std::endl;
  RoundwiseLogWriter::logWrite(log, armNames, armExpectations, policyNames, T, L, "out/"+outfile, true, true);
}

int main(int argc, char *argv[]){
  cmdline::parser a;
  a.add<uint>("runnum", 'r', "run number", false, 100);
  a.add<uint>("mode", 'm', "run mode", false, 1);
  a.add<string>("filename", 'f', "ctr file name", false, "");
  a.add<uint>("roundnum", 't', "round number", false, 10000);
  a.add<int>("seed", 's', "random number seed", false, -1); 
  a.add<string>("outfile", 'o', "output filename", false, "out1.txt");
  a.parse_check(argc, argv);
  const uint RunNum = a.get<uint>("runnum"), T=a.get<uint>("roundnum");
  const uint mode = a.get<uint>("mode");
  const string filename = a.get<string>("filename");
  const string outfile = a.get<string>("outfile"); 

  int rngSeed = a.get<int>("seed"); 
  if(rngSeed != -1){ 
    cout << "rngSeed=" << rngSeed << endl; 
    randomEngine = std::mt19937(rngSeed); 
  } 

  example1(T, RunNum, mode, filename, outfile);

  return 0;
}
