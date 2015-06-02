#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <ctime>
#include <cfloat>

namespace bandit{

typedef unsigned int uint;

const double pi = 3.14159265;
const double e  = 2.718281828;

//RNG engine
std::mt19937 randomEngine(std::time(0));

template<class T>
uint vectorMaxIndex(const std::vector<T> &elems){
  uint m=0;
  T mv=elems[0];
  for(uint i=0;i<elems.size();++i){
    if(elems[i]>mv){
      mv=elems[i];
      m=i;
    }
  }
  return m;
}

template<class T>
std::vector<uint> vectorMaxIndices(const std::vector<T> &elems, uint l){
  std::vector<std::pair<T, uint> > elemPairs;
  for(uint i=0;i<elems.size();++i){
    elemPairs.push_back(std::make_pair(-elems[i],i));
  }
  std::stable_sort(elemPairs.begin(), elemPairs.end());
  std::vector<uint> is;
  uint i=0;
  for(auto it=elemPairs.begin();(it!=elemPairs.end()) && (i<l);++it,++i) {
    is.push_back(it->second);
  }
  return is;
}

template<class T>
T vectorMax(std::vector<T> &elems){
  T mv=elems[0];
  for(unsigned int i=0;i<elems.size();++i){
    if(elems[i]>mv){
      mv=elems[i];
    }
  }
  return mv;
}

template<class T>
T vectorSum(const std::vector<T> &elems){
  T s=0;
  for(uint i=0;i<elems.size();++i){
    s+=elems[i];
  }
  return s;
}

//int -> string
std::string itos(int number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//double -> string
std::string dtos(double number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//randomly sort 1,...,K
std::vector<uint> randomIndices(uint K){
  std::vector<std::pair<double, uint> > temp;
  for(uint i=0;i<K;++i){
    temp.push_back( std::make_pair(std::uniform_real_distribution<double>(0.0,1.0)(randomEngine), i) );
  }
  std::sort(temp.begin(), temp.end());
  std::vector<uint> rvec;
  for(uint i=0;i<K;++i){
    rvec.push_back(temp[i].second);
  }
  return rvec;
}

template<class T>
double kl(T p, T q){
  return p * (log(p) - log(q)) + (1-p) * (log(1-p) - log(1-q));
}


//split by char
std::vector<std::string> split(const std::string &str, char delim){
  std::vector<std::string> res;
  int current = 0, found;
  while((found = str.find_first_of(delim, current)) != (int)(std::string::npos) ){
    res.push_back(std::string(str, current, found - current));
    current = found + 1;
  }
  res.push_back(std::string(str, current, str.size() - current));
  return res;
}

//split by string
std::vector<std::string> split2(const std::string &str, const std::string &delim){
  std::vector<std::string> res;
  int current = 0, found, delimlen = (int)delim.size();
  while((found = str.find(delim, current)) != (int)(std::string::npos)){
    res.push_back(std::string(str, current, found - current));
    current = found + delimlen;
  }
    res.push_back(std::string(str, current, str.size() - current));
    return res;
}

std::vector<std::string> readlines(const std::string filename, bool skipBlankLine = true){
  std::ifstream ifs(filename);
  if(!ifs.is_open()){
    std::cerr << "ERROR: File " << filename << " not found." << std::endl;
    exit(0);
  }
  std::vector<std::string> lines;
  std::string str; 
  while (std::getline(ifs, str))
  {
    if(skipBlankLine && (str.size()==0) ) continue;
    lines.push_back(str);
  } 
  ifs.close();
  return lines;
}

template<class key, class T>
key randomKeyFromMap(const std::map<key, T> &m){
  uint l = m.size();
  uint i = std::uniform_int_distribution<int>(0, l-1)(randomEngine); 
  typename std::map<key, T>::const_iterator it(m.begin());
  std::advance(it, i); 
  return it->first;
}

template<class T>
T randomElementFromSet(const std::set<T> &s){
  uint l = s.size();
  uint i = std::uniform_int_distribution<int>(0, l-1)(randomEngine); 
  typename std::set<T>::const_iterator it(s.begin());
  std::advance(it, i); 
  return *it;
}

template<class T>
std::vector<T> randomElementsFromVector(const std::vector<T> &v, uint num){
  const uint num_check = num;
  if(num>v.size()){
    std::cout << "Error: specified element num is larger than vector size!" << std::endl;
  }
  std::set<uint> S;
  for(uint i=0;i<v.size();++i){
    S.insert(i);
  }
  std::vector<double> elems;
  while(num>0){
    uint i = randomElementFromSet(S);
    S.erase(i);
    elems.push_back(v[i]);
    num--;
  }
  if(elems.size() != num_check){
    std::cout << "Error in randomElementsFromVector" << std::endl;
    exit(0);
  }
  return elems;
}

std::string doubleSetToString(std::set<double> s){
  std::string astr = "";
  for(std::set<double>::iterator it = s.begin(); it != s.end();it++){
    if(astr.length()==0){
      astr += itos(*it);
    }else{
      astr += std::string(" ") + dtos(*it);
    }
  }
  return astr;
}

} //namespace
