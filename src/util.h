#ifndef POWERGRID_UTIL_H_INCLUDED
#define POWERGRID_UTIL_H_INCLUDED

#ifndef MAX_THREADS
#define MAX_THREADS 12
#endif

#include <bitset>
#include <utility>
#include <vector>

class BacktrackingContext {
  public:
    std::bitset<128> covered;
    std::bitset<128> stat;
    BacktrackingContext();
    BacktrackingContext(const BacktrackingContext &other);
};

class Instance {
  public:
    int n;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<int>> g;
    Instance();
    void load(std::ifstream &inf);
    std::string solve();
};

#endif