#pragma once

#include "../bandit/util.hpp"
#include "../bandit/distributions.hpp"

namespace bandit{

//policy base class
class Policy{
public:
  //base functions should not be called
  virtual std::vector<uint> selectNextArms(uint) = 0;
  virtual void updateState(std::vector<uint>, std::vector<double>) = 0;
  virtual std::string toString() = 0;
};

} //namespace
